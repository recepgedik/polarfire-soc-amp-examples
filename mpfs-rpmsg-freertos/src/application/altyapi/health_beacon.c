/*
 * health_beacon.c — emit a SYSTEM_HEALTH (0x02) binary frame on ALTYAPI_UART
 * once per second so the web_arayuz host can observe device liveness.
 *
 * Payload (12 B, little-endian uint32):
 *   system_time_ms : FreeRTOS tick count (configTICK_RATE_HZ == 1000)
 *   reset_count    : bumps each cold boot (no NV storage here, starts at 1)
 *   error_count    : reserved for future error sources, 0 for now
 */

#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

#include "altyapi/altyapi_main.h"
#include "altyapi/health_beacon.h"
#include "altyapi/pfframe.h"
#include "altyapi/versiyon.h"

#define HEALTH_BEACON_PERIOD_MS  1000u
#define HEALTH_PAYLOAD_LEN       12u

static void health_beacon_task(void *pvParameters);

static uint32_t g_reset_count = 1u;
static uint32_t g_error_count = 0u;

void start_health_beacon_task(void)
{
    (void)xTaskCreate(health_beacon_task, "health",
                      configMINIMAL_STACK_SIZE, NULL,
                      PRIMARY_PRIORITY, NULL);
}

static void pack_health_payload(uint8_t out[HEALTH_PAYLOAD_LEN],
                                uint32_t system_time_ms,
                                uint32_t reset_count,
                                uint32_t error_count)
{
    out[0]  = (uint8_t)(system_time_ms       & 0xFFu);
    out[1]  = (uint8_t)((system_time_ms >> 8)  & 0xFFu);
    out[2]  = (uint8_t)((system_time_ms >> 16) & 0xFFu);
    out[3]  = (uint8_t)((system_time_ms >> 24) & 0xFFu);
    out[4]  = (uint8_t)(reset_count          & 0xFFu);
    out[5]  = (uint8_t)((reset_count    >> 8)  & 0xFFu);
    out[6]  = (uint8_t)((reset_count    >> 16) & 0xFFu);
    out[7]  = (uint8_t)((reset_count    >> 24) & 0xFFu);
    out[8]  = (uint8_t)(error_count          & 0xFFu);
    out[9]  = (uint8_t)((error_count    >> 8)  & 0xFFu);
    out[10] = (uint8_t)((error_count    >> 16) & 0xFFu);
    out[11] = (uint8_t)((error_count    >> 24) & 0xFFu);
}

static void health_beacon_task(void *pvParameters)
{
    (void)pvParameters;

    uint8_t frame[PFFRAME_OVERHEAD + HEALTH_PAYLOAD_LEN];
    uint8_t payload[HEALTH_PAYLOAD_LEN];
    TickType_t last_wake = xTaskGetTickCount();

    for (;;)
    {
        uint32_t system_time_ms = (uint32_t)xTaskGetTickCount();

        pack_health_payload(payload, system_time_ms, g_reset_count, g_error_count);

        uint16_t n = pfframe_olustur(frame, (uint16_t)sizeof(frame),
                                     MSG_ID_SYSTEM_HEALTH,
                                     payload, HEALTH_PAYLOAD_LEN);
        if (n > 0u)
        {
            MSS_UART_polled_tx(ALTYAPI_UART, frame, n);
        }

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(HEALTH_BEACON_PERIOD_MS));
    }
}
