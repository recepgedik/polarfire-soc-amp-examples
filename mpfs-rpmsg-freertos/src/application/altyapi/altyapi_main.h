/*
 * altyapi_main.h — entry point and shared UART handle for the altyapi
 * firmware. hart1's u54_1() calls altyapi_start() instead of the Microchip
 * RPMsg demo's start_demo().
 */

#ifndef ALTYAPI_MAIN_H
#define ALTYAPI_MAIN_H

#include "mpfs_hal/mss_hal.h"
#include "drivers/mss/mss_mmuart/mss_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/* UART used by all altyapi modules. Pin routing matches the Microchip RPMsg
 * demo so the same board layout works: MASTER=1 → MMUART1, otherwise MMUART3. */
#ifdef RPMSG_MASTER
#define ALTYAPI_UART        (&g_mss_uart1_lo)
#define ALTYAPI_UART_PERIPH MSS_PERIPH_MMUART1
#else
#define ALTYAPI_UART        (&g_mss_uart3_lo)
#define ALTYAPI_UART_PERIPH MSS_PERIPH_MMUART3
#endif

/* Bring up clocks/UART, spawn altyapi tasks, start the FreeRTOS scheduler.
 * Does not return. */
void altyapi_start(void);

#ifdef __cplusplus
}
#endif

#endif /* ALTYAPI_MAIN_H */
