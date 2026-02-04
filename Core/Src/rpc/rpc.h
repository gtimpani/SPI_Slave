/*
 * rpc_server.h
 *
 *  Created on: 5 dic 2025
 *      Author: Lorenzo Furcas
 */

#ifndef RPC_SERVER_RPC_SERVER_H_
#define RPC_SERVER_RPC_SERVER_H_

#include "rpc_types.h"

/* Initialization function for the rpc server.
 * Takes a reference to a RPCServer object and a sender handler. The sender handler will be used by rpc server to send raw data
 */
RPCError_t rpc_init(RPCServer_t* server, RPCPutSender_t sender_f);


/* Loop function for the server.
 * This function represent the main function of the rpc_server.
 * in is a single byte from input stream.
 * Shall be called on order to make requests execute.
 */
RPCError_t rpc_loop(RPCServer_t* server, uint8_t in);


/* Register a function as rpc callable by server */
RPCError_t rpc_register(RPCServer_t *server, const char *name, RPCFunc fn,
                        RPCParameter_t *input_schema, void *input_data, 
                        RPCParameter_t *output_schema, void *output_data);
                        
#endif /* RPC_SERVER_RPC_SERVER_H_ */
