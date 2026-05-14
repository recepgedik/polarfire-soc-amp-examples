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

#define FIRMWARE_VERSION_STR    "sim-1.0.0"
#define FIRMWARE_VERSION_MAJOR  1u
#define FIRMWARE_VERSION_MINOR  0u
#define FIRMWARE_VERSION_PATCH  0u

#define MSG_ID_TELEMETRY        0x01u
#define MSG_ID_SYSTEM_HEALTH    0x02u
#define MSG_ID_VERSION_QUERY    0x10u
#define MSG_ID_VERSION_RESPONSE 0x11u
#define MSG_ID_STATS_QUERY      0x12u
#define MSG_ID_STATS_RESPONSE   0x13u
#define MSG_ID_RESET            0x14u

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
