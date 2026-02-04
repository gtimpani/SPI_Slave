/*
 * rpc_framing.c
 *
 *  Created on: 5 dic 2025
 *      Author: forre
 */


#include "rpc_framing.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "rpc_types.h"

#define FRAME_MAGIC_L        (0xABu)
#define FRAME_MAGIC_H        (0xCDu)


/* CRC16 helper (CRC-16/IBM style; tweak polynomial if you prefer another) */
static inline uint16_t crc16_update(uint16_t crc, uint8_t data)
{
    uint8_t i;

    crc ^= (uint16_t)data;
    for (i = 0u; i < 8u; i++)
    {
        if ((crc & 1u) != 0u)
        {
            crc = (uint16_t)((crc >> 1) ^ 0xA001u);
        }
        else
        {
            crc >>= 1;
        }
    }
    return crc;
}

static inline uint16_t crc16_compute(const uint8_t *data, uint16_t len)
{
    uint16_t crc = 0u;
    uint16_t i;

    for (i = 0u; i < len; i++)
    {
        crc = crc16_update(crc, data[i]);
    }

    return crc;
}


void rpc_framing_init(RPCFrameParser_t *p)
{
    p->state    = FRAME_PARSER_WAIT_MAGIC_L;
    p->index    = 0u;
    p->crc_calc = 0u;
    p->crc_recv = 0u;
}

/* Function that performs decoding a Simple Framing message
 * Reads byte per byte and return a true if a valid message is found.
 * out is the plain string received
 * len is an i/o parameter. In: max len of out; Out: out written len
 */
bool rpc_framing_decode(RPCFrameParser_t *p, uint8_t b, char *out, size_t *len)
{
	switch (p->state)
	{
		case FRAME_PARSER_WAIT_MAGIC_L:
		{
			if (b == FRAME_MAGIC_L)
			{
				p->state = FRAME_PARSER_WAIT_MAGIC_H;
			}
			/* else stay here */
		} break;

		case FRAME_PARSER_WAIT_MAGIC_H:
		{
			if (b == FRAME_MAGIC_H)
			{
				p->state = FRAME_PARSER_WAIT_LEN_L;
			}
			else
			{
				/* maybe this byte is the start of a new magic */
				if (b == FRAME_MAGIC_L)
				{
					p->state = FRAME_PARSER_WAIT_MAGIC_H;
				}
				else
				{
					p->state = FRAME_PARSER_WAIT_MAGIC_L;
				}
			}
		} break;

		case FRAME_PARSER_WAIT_LEN_L:
		{
			p->len   = (uint16_t)b;     /* low byte */
			p->state = FRAME_PARSER_WAIT_LEN_H;
		} break;

		case FRAME_PARSER_WAIT_LEN_H:
		{
			p->len |= ((uint16_t)b << 8);   /* high byte */
			p->index    = 0u;
			p->crc_calc = 0u;

			if ((p->len == 0u) || (p->len > *len))
			{
				/* invalid length – resync */
				rpc_framing_init(p);
			}
			else
			{
				p->state = FRAME_PARSER_WAIT_PAYLOAD;
			}
		} break;

		case FRAME_PARSER_WAIT_PAYLOAD:
		{
			out[p->index] = b;
			p->crc_calc = crc16_update(p->crc_calc, b);
			p->index++;

			if (p->index >= p->len)
			{
				p->state = FRAME_PARSER_WAIT_CRC_L;
			}
		} break;

		case FRAME_PARSER_WAIT_CRC_L:
		{
			p->crc_recv = (uint16_t)b;   /* low byte */
			p->state    = FRAME_PARSER_WAIT_CRC_H;
		} break;

		case FRAME_PARSER_WAIT_CRC_H:
		{
			p->crc_recv |= ((uint16_t)b << 8);  /* high byte */

			bool frame_ok = (p->crc_calc == p->crc_recv);
			uint16_t frame_len = p->len;        /* save before re-init */

			/* prepare for next frame */
			rpc_framing_init(p);

			if (frame_ok)
			{
				*len = frame_len;
				return true;    /* full valid frame ready in p->payload / p->len */
			}
			/* CRC error discard frame, wait for next magic */
		} break;

		default:
		{
			rpc_framing_init(p);
		} break;
	}

	return false;   /* frame not complete yet */

}

/* Function performing encoding of a string of size len.
 * buff is the output buffer
 * len is an i/o parameter. In: max len of out; Out: out written len
 */
bool rpc_framing_encode(const char *in, size_t len, uint8_t *buff, size_t *out_len)
{
	uint16_t needed;
	uint16_t i;
	uint16_t crc;
	size_t frame_buf_size = *out_len;

	if ((in == NULL) || (buff == NULL) || (*out_len == 0u) || (len == 0u))
	{
		return false;
	}

	/* 2 magic + 2 len + payload + 2 CRC */
	needed = (uint16_t)(len + 6u);
	if (frame_buf_size < needed)
	{
		return false;
	}

	//* Build header */
    buff[0] = FRAME_MAGIC_L;
    buff[1] = FRAME_MAGIC_H;
    buff[2] = (uint8_t)(len & 0xFFu);          /* LEN_L */
    buff[3] = (uint8_t)((len >> 8) & 0xFFu);   /* LEN_H */

    /* Copy payload */
    for (i = 0u; i < len; i++)
    {
        buff[4u + i] = (uint8_t)in[i];
    }

    /* Compute CRC over payload only (same as parser) */
    crc = crc16_compute((const uint8_t *)in, (uint16_t)len);
    buff[4u + len] = (uint8_t)(crc & 0xFFu);          /* CRC_L */
    buff[5u + len] = (uint8_t)((crc >> 8) & 0xFFu);   /* CRC_H */

    *out_len = needed;
	return true;
}
