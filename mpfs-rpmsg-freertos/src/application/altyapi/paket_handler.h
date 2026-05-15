/*
 * paket_handler.h — RX yolu + paket dispatcher.
 *
 * Task ALTYAPI_UART'tan gelen byte'ları ring buffer'a alır, pfframe_coz
 * ile frame'leri parse eder ve msg_id'ye göre handler'a yönlendirir.
 * Şu anda PAKET_VERSION_QUERY işlenir (response yollar).
 */

#ifndef PAKET_HANDLER_H
#define PAKET_HANDLER_H

#ifdef __cplusplus
extern "C" {
#endif

/* RX + dispatcher task'ını spawn et. vTaskStartScheduler'dan önce çağır. */
void start_paket_handler_task(void);

#ifdef __cplusplus
}
#endif

#endif /* PAKET_HANDLER_H */
