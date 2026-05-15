#ifndef RECEIVER_WORKER_H
#define RECEIVER_WORKER_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cmsis_os2.h"
#include "gpio_sun.h"
#include "receiver_base.h"

/* Константы сигналов */
#define SIG_REC_NEW_CMD     0x00000001
#define SIG_REC_TIMER_DONE  0x00000002
#define SIG_DMA_COMPLETE    0x00000004


/* 3. Контекст воркера приемника (Диспетчер) */
typedef struct receiver_worker_ctx_s {
    uint8_t             id;             // ID воркера (0 или 1)
    osEventFlagsId_t    evt_id;         // Флаги событий (Команды, Таймер, DMA)
    osMessageQueueId_t  q_id;           // Очередь входящих команд
    osTimerId_t         scan_timer_id;  // Таймер кванта времени

    uint32_t            scan_step_ms;   // Величина одного "тика" (delta_ms)

    receiver_base_t		base;

} receiver_worker_ctx_t;

/* Глобальные функции */
void receiver_worker_task(void *argument);

#endif // RECEIVER_WORKER_H
