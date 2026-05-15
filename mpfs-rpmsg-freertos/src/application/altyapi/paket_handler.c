/*
 * paket_handler.c — gomulu_paket_isakisi.md §"Ekle/Gönder/Al/Çıkar"
 * akışının kod karşılığı. RX task UART'ı yoklar, byte'ları ring buffer'a
 * alır, lib'in pfframe_coz stream parser'ına devreder ve decoded frame'i
 * dispatcher'a yönlendirir.
 *
 * Şu an handle edilen paketler:
 *   PAKET_VERSION_QUERY  → PAKET_VERSION_RESPONSE (FIRMWARE_VERSION_STR)
 */

#include <stdint.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "altyapi/altyapi_main.h"
#include "altyapi/paket_handler.h"
#include "altyapi/versiyon.h"

#include "pfframe.h"  /* protokol_lib */

#define RX_BUF_SIZE         128u
#define RX_POLL_PERIOD_MS   10u
#define RX_CHUNK_MAX        32u
#define VERSION_OUT_BUF     (PFFRAME_OVERHEAD + 32u)

static uint8_t  s_rx_buf[RX_BUF_SIZE];
static uint16_t s_rx_len = 0u;

static void paket_handler_task(void *pvParameters);
static void drain_uart_into_buffer(void);
static void rx_consume(uint16_t n);
static void handle_packet(uint8_t msg_id,
                          const uint8_t *payload,
                          uint16_t payload_len);
static void send_version_response(void);

void start_paket_handler_task(void)
{
    (void)xTaskCreate(paket_handler_task, "paket",
                      configMINIMAL_STACK_SIZE, NULL,
                      PRIMARY_PRIORITY, NULL);
}

static void paket_handler_task(void *pvParameters)
{
    (void)pvParameters;
    TickType_t last_wake = xTaskGetTickCount();

    for (;;)
    {
        drain_uart_into_buffer();

        /* Lib stream parser: garbage skip + header/length/footer + resync
         * lib içinde; biz sadece sonucu işliyoruz. Birden fazla frame
         * birikmişse loop ile hepsini boşalt. */
        for (;;)
        {
            pfframe_result_t r = pfframe_coz(s_rx_buf, s_rx_len);
            if (r.frame_ok)
            {
                /* payload pointer s_rx_buf'a işaret eder — rx_consume'dan
                 * ÖNCE handler'a ver. */
                handle_packet(r.msg_id, r.payload, r.payload_len);
            }
            if (r.consumed == 0u)
            {
                break;  /* daha veri yok / tam frame henüz gelmedi */
            }
            rx_consume(r.consumed);
            if (!r.frame_ok)
            {
                break;  /* sadece garbage temizlendi, buffer şu an boş ya da yetersiz */
            }
        }

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(RX_POLL_PERIOD_MS));
    }
}

static void drain_uart_into_buffer(void)
{
    while (s_rx_len < RX_BUF_SIZE)
    {
        uint8_t  chunk[RX_CHUNK_MAX];
        uint16_t cap = (uint16_t)(RX_BUF_SIZE - s_rx_len);
        if (cap > sizeof(chunk))
        {
            cap = sizeof(chunk);
        }
        size_t got = MSS_UART_get_rx(ALTYAPI_UART, chunk, cap);
        if (got == 0u)
        {
            return;
        }
        memcpy(&s_rx_buf[s_rx_len], chunk, got);
        s_rx_len += (uint16_t)got;
    }
}

/* memmove dışı: -nodefaultlibs ile libc çekilmiyor. */
static void rx_consume(uint16_t n)
{
    if (n >= s_rx_len)
    {
        s_rx_len = 0u;
        return;
    }
    uint16_t remaining = (uint16_t)(s_rx_len - n);
    for (uint16_t i = 0u; i < remaining; i++)
    {
        s_rx_buf[i] = s_rx_buf[i + n];
    }
    s_rx_len = remaining;
}

static void handle_packet(uint8_t msg_id,
                          const uint8_t *payload,
                          uint16_t payload_len)
{
    (void)payload;

    if (msg_id == (uint8_t)PAKET_VERSION_QUERY)
    {
        if (payload_len != 0u)
        {
            return;  /* spec: query empty payload */
        }
        send_version_response();
    }
    /* Diğer paketler şimdilik no-op. */
}

static void send_version_response(void)
{
    const char *v    = FIRMWARE_VERSION_STR;
    uint16_t    vlen = (uint16_t)strlen(v);

    uint8_t out[VERSION_OUT_BUF];
    if (vlen > (uint16_t)(sizeof(out) - PFFRAME_OVERHEAD))
    {
        vlen = (uint16_t)(sizeof(out) - PFFRAME_OVERHEAD);
    }

    uint16_t n = pfframe_olustur(out, (uint16_t)sizeof(out),
                                 (uint8_t)PAKET_VERSION_RESPONSE,
                                 (const uint8_t *)v, vlen);
    if (n > 0u)
    {
        MSS_UART_polled_tx(ALTYAPI_UART, out, n);
    }
}
