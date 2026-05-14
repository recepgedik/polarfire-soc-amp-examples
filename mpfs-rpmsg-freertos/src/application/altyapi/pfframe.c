/*
 * pfframe.c — implementation of the AA 55 binary frame pack / parse helpers.
 * See pfframe.h for the API contract and versiyon.h for the wire constants.
 */

#include <stddef.h>
#include <string.h>

#include "altyapi/pfframe.h"
#include "altyapi/versiyon.h"

uint16_t pfframe_olustur(uint8_t *out, uint16_t out_cap,
                         uint8_t msg_id,
                         const uint8_t *payload,
                         uint16_t payload_len)
{
    uint32_t frame_len = (uint32_t)payload_len + (uint32_t)PFFRAME_OVERHEAD;

    if ((out == NULL) || (frame_len > (uint32_t)out_cap))
    {
        return 0u;
    }

    out[0] = PFFRAME_HEADER_1;
    out[1] = PFFRAME_HEADER_2;
    out[2] = msg_id;
    out[3] = (uint8_t)(payload_len & 0xFFu);
    out[4] = (uint8_t)((payload_len >> 8) & 0xFFu);

    if ((payload_len > 0u) && (payload != NULL))
    {
        memcpy(&out[5], payload, (size_t)payload_len);
    }

    out[5u + payload_len] = PFFRAME_FOOTER_CR;
    out[6u + payload_len] = PFFRAME_FOOTER_LF;

    return (uint16_t)frame_len;
}

uint16_t pfframe_coz(const uint8_t *in, uint16_t in_len,
                     uint8_t *msg_id_out,
                     const uint8_t **payload_out,
                     uint16_t *payload_len_out)
{
    if ((in == NULL) || (in_len < PFFRAME_OVERHEAD))
    {
        return 0u;
    }
    if ((in[0] != PFFRAME_HEADER_1) || (in[1] != PFFRAME_HEADER_2))
    {
        return 0u;
    }

    uint16_t payload_len = (uint16_t)in[3] | ((uint16_t)in[4] << 8);
    uint32_t total_len   = (uint32_t)payload_len + (uint32_t)PFFRAME_OVERHEAD;

    if (total_len != (uint32_t)in_len)
    {
        return 0u;
    }
    if ((in[5u + payload_len] != PFFRAME_FOOTER_CR) ||
        (in[6u + payload_len] != PFFRAME_FOOTER_LF))
    {
        return 0u;
    }

    if (msg_id_out != NULL)
    {
        *msg_id_out = in[2];
    }
    if (payload_len_out != NULL)
    {
        *payload_len_out = payload_len;
    }
    if (payload_out != NULL)
    {
        *payload_out = (payload_len > 0u) ? &in[5] : NULL;
    }

    return (uint16_t)total_len;
}
