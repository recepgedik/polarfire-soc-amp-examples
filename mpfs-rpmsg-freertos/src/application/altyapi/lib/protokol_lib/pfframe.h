/*
 * pfframe.h — PolarFire binary frame serializer / stream parser.
 *
 * Self-contained wire-format library. Applications that use this lib
 * supply their own msg_id catalog and payload structs.
 *
 * Wire layout:
 *   [0]      0xAA          header byte 1
 *   [1]      0x55          header byte 2
 *   [2]      msg_id  u8
 *   [3..4]   length  u16   little-endian, payload size N
 *   [5..5+N) payload bytes
 *   [5+N]    0x0D          footer CR
 *   [5+N+1]  0x0A          footer LF
 *
 * Total frame size = PFFRAME_OVERHEAD + N.
 */

#ifndef PFFRAME_H
#define PFFRAME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------------
 * Library version
 * -------------------------------------------------------------------------- */

#define PFFRAME_VERSION_MAJOR  0u
#define PFFRAME_VERSION_MINOR  1u
#define PFFRAME_VERSION_PATCH  0u
#define PFFRAME_VERSION_STR    "0.1.0"

/* --------------------------------------------------------------------------
 * Wire format constants
 * -------------------------------------------------------------------------- */

#define PFFRAME_HEADER_1     0xAAu
#define PFFRAME_HEADER_2     0x55u
#define PFFRAME_FOOTER_CR    0x0Du
#define PFFRAME_FOOTER_LF    0x0Au

#define PFFRAME_OVERHEAD     7u
#define PFFRAME_MAX_PAYLOAD  0xFFFFu

/* --------------------------------------------------------------------------
 * Serialize
 * -------------------------------------------------------------------------- */

/* Pack a frame into out[]. Returns the total frame length on success,
 * or 0 if the output buffer cannot hold PFFRAME_OVERHEAD + payload_len bytes. */
uint16_t pfframe_olustur(uint8_t *out, uint16_t out_cap,
                         uint8_t msg_id,
                         const uint8_t *payload,
                         uint16_t payload_len);

/* --------------------------------------------------------------------------
 * Stream parser
 * -------------------------------------------------------------------------- */

/* Result of one pfframe_coz call. */
typedef struct {
    uint16_t       consumed;     /* bytes the caller should drop from the head of buf */
    uint16_t       payload_len;  /* valid when frame_ok == 1 */
    uint16_t       errors;       /* garbage/broken bytes skipped during THIS call */
    uint8_t        msg_id;       /* valid when frame_ok == 1 */
    uint8_t        frame_ok;     /* 1 = frame decoded, 0 = no complete frame this call */
    const uint8_t *payload;      /* valid when frame_ok == 1, points inside buf (zero-copy) */
} pfframe_result_t;

/* Scan buf[] for the next valid frame. Skips garbage bytes ahead of the
 * 0xAA 0x55 sync, and recovers automatically from a bad footer by advancing
 * one byte and trying again.
 *
 * Behavior:
 *   - No header in buffer        -> consumed = leading garbage bytes (may keep
 *                                   a trailing 0xAA for the next call),
 *                                   frame_ok = 0.
 *   - Header found but truncated -> consumed = leading garbage (header byte
 *                                   preserved for the next call), frame_ok = 0.
 *   - Header + complete + valid  -> consumed = leading garbage + frame total,
 *                                   frame_ok = 1, msg_id/payload/payload_len set.
 *   - Header + footer mismatch   -> counted in errors, parser advances one byte
 *                                   and continues searching.
 *
 * Stats (toplam_okunan_bayt / basarili_paket_sayisi / basarisiz_paket_sayisi)
 * are updated internally; query them with pfframe_istatistik_ogren(). */
pfframe_result_t pfframe_coz(const uint8_t *buf, uint16_t buf_len);

/* --------------------------------------------------------------------------
 * Istatistik (counters)
 * -------------------------------------------------------------------------- */

/* Wire-format aligned with web_arayuz claud.md §3 PAKET_STATS_RESPONSE. */
typedef struct {
    uint32_t toplam_okunan_bayt;      /* bytes belonging to successfully parsed frames */
    uint32_t basarili_paket_sayisi;   /* number of valid frames decoded */
    uint32_t basarisiz_paket_sayisi;  /* number of bad/garbage bytes encountered */
} pfframe_istatistik_t;

/* Copy a snapshot of the current counters into *out. */
void pfframe_istatistik_ogren(pfframe_istatistik_t *out);

/* Zero all counters. Useful for the soft-reset handler. */
void pfframe_istatistik_sifirla(void);

#ifdef __cplusplus
}
#endif

#endif /* PFFRAME_H */
