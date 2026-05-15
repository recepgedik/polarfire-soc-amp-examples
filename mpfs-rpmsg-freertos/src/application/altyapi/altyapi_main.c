/*
 * altyapi_main.c — altyapi firmware entry point.
 *
 * Replaces the Microchip RPMsg demo's start_demo(): does only what altyapi
 * needs — peripheral clocks, PLIC, UART, the health-beacon task, then the
 * FreeRTOS scheduler. The RPMsg demo files (demo_main.c, *_demo.c) are
 * still compiled but no longer reachable at runtime.
 */

#include "FreeRTOS.h"
#include "task.h"

#include "altyapi/altyapi_main.h"
#include "altyapi/health_beacon.h"
#include "altyapi/paket_handler.h"

void altyapi_start(void)
{
    (void)mss_config_clk_rst(ALTYAPI_UART_PERIPH,
                             (uint8_t)MPFS_HAL_FIRST_HART,
                             PERIPHERAL_ON);

    PLIC_init();

    MSS_UART_init(ALTYAPI_UART, MSS_UART_115200_BAUD,
                  MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY);

    start_health_beacon_task();
    start_paket_handler_task();

    vTaskStartScheduler();

    /* vTaskStartScheduler only returns on heap exhaustion. */
    for (;;) { }
}
