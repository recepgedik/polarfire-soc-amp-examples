/*
 * versiyon.h — Polarfire Altyapı firmware version & message catalog.
 *
 * Wire-format constants (PFFRAME_HEADER_*, PFFRAME_OVERHEAD, ...) live in
 * the external protokol_lib (pfframe.h). This file keeps only the bits
 * specific to this firmware: identity strings, message IDs, payload structs.
 */

#ifndef VERSIYON_H
#define VERSIYON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define FIRMWARE_VERSION_STR    "1.0.0"
#define FIRMWARE_VERSION_MAJOR  1u
#define FIRMWARE_VERSION_MINOR  0u
#define FIRMWARE_VERSION_PATCH  0u

/* Paket tipleri — pfframe başlığındaki msg_id alanında taşınan değerler.
 * Boş bir paket göndermek anlamlı olmadığından PAKET_NONE = 0 reserve. */
typedef enum {
    PAKET_NONE             = 0x00u,
    PAKET_TELEMETRY        = 0x01u,
    PAKET_SYSTEM_HEALTH    = 0x02u,
    PAKET_VERSION_QUERY    = 0x10u,
    PAKET_VERSION_RESPONSE = 0x11u,
    PAKET_STATS_QUERY      = 0x12u,
    PAKET_STATS_RESPONSE   = 0x13u,
    PAKET_RESET            = 0x14u,
} paket_tipi_t;

typedef struct {
    float pitch;
    float roll;
} payload_telemetry_t;

typedef struct {
    uint32_t system_time_ms;
    uint32_t reset_count;
    uint32_t error_count;
} payload_health_t;

typedef struct {
    uint32_t received_bytes;
    uint32_t success_count;
    uint32_t error_count;
} payload_stats_t;

#ifdef __cplusplus
}
#endif

#endif /* VERSIYON_H */
