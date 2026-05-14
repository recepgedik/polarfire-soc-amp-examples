/*
 * versiyon.h — Polarfire Altyapı firmware version & protocol constants.
 *
 * Mirror of core/protocol.py in the web_arayuz project (claud.md §2/§3).
 * Hand-synced — both files must change together.
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

#define PFFRAME_HEADER_1   0xAAu
#define PFFRAME_HEADER_2   0x55u
#define PFFRAME_FOOTER_CR  0x0Du
#define PFFRAME_FOOTER_LF  0x0Au

#define PFFRAME_OVERHEAD       7u
#define PFFRAME_MAX_PAYLOAD    0xFFFFu

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
