/*
 * rpc_framing.h
 *
 *  Created on: 5 dic 2025
 *      Author: forre
 */

#ifndef RPC_SERVER_RPC_FRAMING_H_
#define RPC_SERVER_RPC_FRAMING_H_

#include <stdbool.h>
#include "rpc_types.h"



void rpc_framing_init(RPCFrameParser_t *p);

/* Function that performs decoding a Simple Framing message
 * Reads byte per byte and return a true if a valid message is found.
 * out is the plain string received
 * len is an i/o parameter. In: max len of out; Out: out written len
 */
bool rpc_framing_decode(RPCFrameParser_t *p, uint8_t in, char *out, size_t *len);

/* Function performing encoding of a string of size len.
 * buff is the output buffer with length len
 * out_len is an i/o parameter. In: max len of out; Out: out written len
 */
bool rpc_framing_encode(const char *in, size_t len, uint8_t *buff, size_t *out_len);

#endif /* RPC_SERVER_RPC_FRAMING_H_ */
