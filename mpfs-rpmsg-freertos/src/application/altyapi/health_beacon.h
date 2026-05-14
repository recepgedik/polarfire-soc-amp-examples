/*
 * health_beacon.h — 1 Hz SYSTEM_HEALTH (0x02) emitter over UART_APP.
 */

#ifndef HEALTH_BEACON_H
#define HEALTH_BEACON_H

#ifdef __cplusplus
extern "C" {
#endif

/* Spawn the health-beacon FreeRTOS task. Call before vTaskStartScheduler(). */
void start_health_beacon_task(void);

#ifdef __cplusplus
}
#endif

#endif /* HEALTH_BEACON_H */
