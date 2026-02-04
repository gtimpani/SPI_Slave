/*
 * rpc_server.c
 *
 *  Created on: 5 dic 2025
 *      Author: forre
 */

#include "rpc.h"
#include "rpc_framing.h"
#include <stdlib.h>
#include <string.h>

#define RPC_VERSION "2.0"

#define RPC_ASSERT(predicate, err)                                             \
  do {                                                                         \
    if (!(predicate))                                                          \
      return (err);                                                            \
  } while (0)

static inline int MIN(int x, int y) { return (x <= y) ? x : y; }

static char rpc_error_buffer[RPC_MAX_ERROR_MSG_LEN];

static const char *rpc_error_to_str(RPCError_t e) {
  switch (e) {
  case RPC_PARSE_ERROR:
    return "INVALID JSON";

  case RPC_INVALID_REQUEST:
    return "INVALID_REQUEST";

  case RPC_METHOD_NOT_FOUND:
    return "METHOD_NOT_FOUND";

  case RPC_INVALID_PARAMS:
    return "INVALID_PARAMS";

  case RPC_INTERNAL_ERROR:
    return "INTERNAL_ERROR";

  case RPC_SUCCESS:
    return "SUCCESS";

  default:
    return "INTERNAL_ERROR";
  }
}

static RPCError_t rpc_request_decode(char *json_str, size_t len, json_t *mem,
                                     size_t max_tokens, RPCRequest_t *req) {

  /* Clear the request object */
  memset(req, 0, sizeof(RPCRequest_t));

  json_t const *json = json_create(json_str, mem, max_tokens);
  RPC_ASSERT(json, RPC_PARSE_ERROR);

  json_t const *_jsonrpc_tok = json_getProperty(json, "jsonrpc");
  req->version = json_getValue(_jsonrpc_tok);
  RPC_ASSERT(_jsonrpc_tok, RPC_PARSE_ERROR);
  req->version_len = strlen(req->version);

  json_t const *_id_tok = json_getProperty(json, "id");
  req->id = json_getInteger(_id_tok);
  RPC_ASSERT(_id_tok, RPC_PARSE_ERROR);

  json_t const *_method_tok = json_getProperty(json, "method");
  req->method = json_getValue(_method_tok);
  RPC_ASSERT(_method_tok, RPC_PARSE_ERROR);
  req->method_len = strlen(req->method);

  // Store the handle to the "params" object/array directly
  req->params = json_getProperty(json, "params");

  return RPC_SUCCESS;
}

static RPCError_t rpc_request_validate(RPCRequest_t *req) {
  size_t vlen = strlen(RPC_VERSION);

  RPC_ASSERT((req->version_len == vlen) &&
                 (strncmp(req->version, RPC_VERSION, vlen) == 0),
             RPC_INVALID_REQUEST);

  return RPC_SUCCESS;
}

static RPCError_t rpc_find_entry(RPCEntry_t *t, size_t l, const char *name,
                                 size_t nl, RPCEntry_t **e) {
  for (size_t i = 0u; i < l; ++i) {
    size_t entry_len = strlen(t[i].name);
    if (entry_len == nl && strncmp(t[i].name, name, nl) == 0) {
      *e = &t[i];
      return RPC_SUCCESS;
    }
  }
  return RPC_METHOD_NOT_FOUND;
}

static RPCError_t rpc_decode_params(const RPCRequest_t *req,
                                    RPCEntry_t *entry) {
  if ((entry == NULL) || (entry->input.param == NULL)) {
    return RPC_SUCCESS;
  }

  RPC_ASSERT(req->params != NULL, RPC_INVALID_PARAMS);

  /* req->params is already the JSON object/array node */
  RPCParameter_t root = {.type = RPC_OBJECT,
                         .sub.obj_fields = entry->input.param};
  RPCResult_t res = json_parse(req->params, &root, entry->input.data);

  if (res != RPC_OK) {
    return RPC_INVALID_PARAMS;
  }

  return RPC_SUCCESS;
}

static RPCError_t rpc_execute(RPCServer_t *s, RPCRequest_t *req,
                              RPCResponse_t *res) {
  RPCError_t e = RPC_SUCCESS;
  RPCEntry_t *entry = NULL;

  res->error.code = 0;
  res->error.message[0] = 0;

  //    res->result = NULL;

  e = rpc_find_entry(s->entry_table, s->entry_len, req->method, req->method_len,
                     &entry);
  if ((e == RPC_SUCCESS) && (entry != NULL)) {
    e = rpc_decode_params(req, entry);
    if (e == RPC_SUCCESS) {
      res->error.code = entry->fn(res->error.message);
      res->result.data = entry->output.data;
      res->result.param = entry->output.param;

      if (res->error.code == 0) {
        // res->result = entry->outs;
      }
    }
  }

  return e;
}

static RPCError_t rpc_response_encode(RPCResponse_t *res, uint8_t *buff,
                                      size_t *l, RPCError_t e) {
  /* Choose what goes into the "error" object */

  int code_val;
  const char *msg_val;
  int w = 0;

  if (e == RPC_SUCCESS) {
    code_val = res->error.code;
    msg_val = res->error.message;
  } else {
    code_val = (int)e;
    msg_val = rpc_error_to_str(e);
  }

  char *p = (char *)buff + 4;
  size_t start_len = RPC_SERVER_MAX_MSG_LEN;
  size_t rem_len = start_len;

  p = json_objOpen(p, NULL, &rem_len);
  p = json_str(p, "jsonrpc", RPC_VERSION, &rem_len);
  p = json_int(p, "id", res->id, &rem_len);

  p = json_objOpen(p, "result", &rem_len);
  w = json_dump(res->result.data, res->result.param, p, rem_len);
  p += w;
  rem_len -= w;
  p = json_objClose(p, &rem_len);

  if (res->error.code != 0) {
    p = json_objOpen(p, "error", &rem_len);
    p = json_int(p, "code", res->error.code, &rem_len);
    /* To use later for safety */
    res->error.message[100] = 0;
    p = json_str(p, "message", res->error.message, &rem_len);
    p = json_objClose(p, &rem_len);
  }

  p = json_objClose(p, &rem_len);
  p = json_end(p, &rem_len);

  *l = start_len - rem_len;

  /* JSON into buff+4 (leave room for framing header) */
  // RPC_ASSERT(
  //     c_2_json((char *)buff + 4, RPC_SERVER_MAX_MSG_LEN - 4, _json_response)
  //     == 0, RPC_INTERNAL_ERROR
  // );

  /* Frame it: use full buffer size as capacity */
  size_t msg_cap = RPC_SERVER_MAX_MSG_LEN;
  RPC_ASSERT(rpc_framing_encode((char *)buff + 4, *l, buff, &msg_cap),
             RPC_INTERNAL_ERROR);

  *l = msg_cap;

  return RPC_SUCCESS;
}

/* Initialization function for the rpc server.
 * Takes a reference to a RPCServer object and a sender handler. The sender
 * handler will be used by rpc server to send raw data
 */
RPCError_t rpc_init(RPCServer_t *server, RPCPutSender_t sender_f) {
  server->sender_f = sender_f;
  server->to_send = 0;
  server->entry_len = 0;
  server->res.error.code = 0;
  server->res.error.message[0] = 0;
  return RPC_SUCCESS;
}

/* Loop function for the server.
 * This function represent the main function of the rpc_server.
 * in is a single byte from input stream.
 * Shall be called on order to make requests execute.
 */
RPCError_t rpc_loop(RPCServer_t *server, uint8_t in) {
  RPCError_t err = RPC_SUCCESS;

  size_t len = RPC_SERVER_MAX_MSG_LEN;
  if (rpc_framing_decode(&server->frame_p, in, (char *)server->rx_buff, &len)) {
    /* Decode */
    err = rpc_request_decode((char *)server->rx_buff, len, server->tokens,
                             RPC_JSON_TOKEN_MAX, &server->req);
    if (err == RPC_SUCCESS) {
      /* Copy id to response so we always answer with same id */
      server->res.id = server->req.id;

      /* Validate */
      err = rpc_request_validate(&server->req);
      if (err == RPC_SUCCESS) {
        /* Execute */
        err = rpc_execute(server, &server->req, &server->res);
      }
    }

    /* Encode response (always try, even on error) */
    rpc_response_encode(&server->res, server->tx_buff, &server->to_send, err);
  }

  /* If there is something in tx_buffer send it */
  if (server->to_send > 0u) {
    server->sender_f(server->tx_buff, server->to_send);
    server->to_send = 0u;
  }

  return err;
}

/* Register a function as rpc callable by server */
RPCError_t rpc_register(RPCServer_t *server, const char *name, RPCFunc fn,
                        RPCParameter_t *input_schema, void *input_data,
                        RPCParameter_t *output_schema, void *output_data) {
  RPCEntry_t *t = server->entry_table;
  uint16_t l = server->entry_len;

  if (l >= RPC_SERVER_MAX_ENTRY_NUM) {
    return RPC_TABLE_FULL;
  }

  t[l].name = name;
  t[l].fn = fn;
  t[l].input.param = input_schema;
  t[l].input.data = input_data;
  t[l].output.param = output_schema;
  t[l].output.data = output_data;

  server->entry_len++;
  return 0;
}
