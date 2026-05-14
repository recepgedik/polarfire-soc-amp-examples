/*
 * pfframe.h — PolarFire binary frame serializer / parser.
 *
 * Wire layout (see versiyon.h / claud.md §2):
 *   [0]      0xAA          header byte 1
 *   [1]      0x55          header byte 2
 *   [2]      msg_id  u8
 *   [3..4]   length  u16   little-endian, payload size N
 *   [5..5+N) payload bytes
 *   [5+N]    0x0D          footer CR
 *   [5+N+1]  0x0A          footer LF
 */

#ifndef PFFRAME_H
#define PFFRAME_H

#include <stdint.h>

#include "altyapi/versiyon.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Serialize a frame into out[]. Returns the total frame length on success,
 * or 0 if the output buffer cannot hold PFFRAME_OVERHEAD + payload_len bytes. */
uint16_t pfframe_olustur(uint8_t *out, uint16_t out_cap,
                         uint8_t msg_id,
                         const uint8_t *payload,
                         uint16_t payload_len);

/* Validate and decode a single complete frame in `in[]`. The decoded
 * `*payload_out` points inside `in` (no copy) so the caller must keep
 * the input buffer alive while reading the payload.
 *
 * Returns the total frame length on success, or 0 if the input is not
 * a valid frame (bad header, length mismatch, bad footer, buffer too short).
 * Output parameters are only written on success and may be NULL. */
uint16_t pfframe_coz(const uint8_t *in, uint16_t in_len,
                     uint8_t *msg_id_out,
                     const uint8_t **payload_out,
                     uint16_t *payload_len_out);

#ifdef __cplusplus
}
#endif

#endif /* PFFRAME_H */
