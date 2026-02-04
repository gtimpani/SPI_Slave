/*
 * rpc_types.h
 *
 *  Created on: 5 dic 2025
 *      Author: forre
 */

#ifndef RPC_SERVER_RPC_TYPES_H_
#define RPC_SERVER_RPC_TYPES_H_

#ifndef RPC_SERVER_MAX_MSG_LEN
#define RPC_SERVER_MAX_MSG_LEN 4096
#endif

#ifndef RPC_SERVER_MAX_ENTRY_NUM
#define RPC_SERVER_MAX_ENTRY_NUM 1024
#endif

#ifndef RPC_MAX_ERROR_MSG_LEN
#define RPC_MAX_ERROR_MSG_LEN 128
#endif

#ifndef RPC_JSON_TOKEN_MAX
#define RPC_JSON_TOKEN_MAX 512
#endif

#include "json/tz_json.h"
#include <stddef.h>
#include <stdint.h>

typedef void (*RPCPutSender_t)(uint8_t *buf, size_t len);
typedef int (*RPCFunc)(char *e_msg);
typedef struct json_attr_t RPCJsonField;

typedef enum {
  RPC_PARSE_ERROR = -32700,
  RPC_INVALID_REQUEST = -32600,
  RPC_METHOD_NOT_FOUND = -32601,
  RPC_INVALID_PARAMS = -32602,
  RPC_INTERNAL_ERROR = -32603,
  RPC_TABLE_FULL = -32200,
  RPC_SUCCESS = 0,

} RPCError_t;

/* Only for internal usage
 *
 */
typedef enum {
  FRAME_PARSER_WAIT_MAGIC_L = 0,
  FRAME_PARSER_WAIT_MAGIC_H,
  FRAME_PARSER_WAIT_LEN_L,
  FRAME_PARSER_WAIT_LEN_H,
  FRAME_PARSER_WAIT_PAYLOAD,
  FRAME_PARSER_WAIT_CRC_L,
  FRAME_PARSER_WAIT_CRC_H

} RPCFrameParserState_t;

typedef struct {
  RPCFrameParserState_t state;
  uint16_t len;      /* expected payload length (16-bit) */
  uint16_t index;    /* bytes collected so far (16-bit) */
  uint16_t crc_calc; /* running CRC16 over payload */
  uint16_t crc_recv; /* CRC received from stream */

} RPCFrameParser_t;

typedef struct {
  const char *version;
  uint16_t version_len;
  int id;
  const char *method;
  uint16_t method_len;
  json_t const *params;

} RPCRequest_t;

typedef struct {
  int id;
  struct {
    int code;
    char message[512];
  } error;
  struct {
	  RPCParameter_t *param;
	  void *data;
  } result;
} RPCResponse_t;

typedef struct {
  const char *name;
  RPCFunc fn;
  struct {
    RPCParameter_t *param;
    void *data;
  } input;
  struct {
    RPCParameter_t *param;
    void *data;
  } output;

} RPCEntry_t;

typedef struct {
  RPCPutSender_t sender_f;
  uint8_t rx_buff[RPC_SERVER_MAX_MSG_LEN];
  uint8_t tx_buff[RPC_SERVER_MAX_MSG_LEN];
  size_t to_send;
  RPCFrameParser_t frame_p;
  json_t tokens[RPC_JSON_TOKEN_MAX];
  RPCRequest_t req;
  RPCResponse_t res;
  RPCEntry_t entry_table[RPC_SERVER_MAX_ENTRY_NUM];
  uint16_t entry_len;

} RPCServer_t;

#endif /* RPC_SERVER_RPC_TYPES_H_ */
