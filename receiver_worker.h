#ifndef RECEIVER_WORKER_H
#define RECEIVER_WORKER_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cmsis_os2.h"
#include "gpio_sun.h"

/* Константы сигналов */
#define SIG_REC_NEW_CMD     0x00000001
#define SIG_REC_TIMER_DONE  0x00000002
#define SIG_DMA_COMPLETE    0x00000004

#define MAX_DSP_SAMPLES     1024

/* 1. Менеджер общего физического ресурса АЦП */
typedef struct {
    osMutexId_t mutex;
    bool        is_initialized;
} gpadc_manager_t;

/* 2. DSP Анализатор — инкапсулирован в структуру воркера (Матрешка) */
typedef struct {
    // Настройки анализа
    uint16_t n_samples;      // Размер выборки (N)
    uint16_t n_repeats;      // Количество усреднений (n)
    uint16_t target_bin;     // Целевая частота (15.6 кГц)
    float    threshold_mag;  // Порог магнитуды
    float    threshold_snr;  // Порог сигнал/шум

    // Параметры аналогового ключа (FSA3157)
    struct gpio_t_ sw_pin;   // Пин управления (порт, пин)
    uint8_t        sw_state; // Состояние (0 или 1) для подключения ЭТОГО тюнера

    // Результаты последнего измерения
    float    last_mag;
    float    last_snr;
    bool     is_video_found;

    // Буфер АЦП — выровнен по 64 байта для корректной работы кэша T113-S3
//    __attribute__((aligned(64)))
#pragma data_alignment=64
    uint16_t adc_buffer[MAX_DSP_SAMPLES];
} receiver_dsp_t;

/* 3. Контекст воркера приемника (Диспетчер) */
typedef struct receiver_worker_ctx_s {
    uint8_t             id;             // ID воркера (0 или 1)
    osEventFlagsId_t    evt_id;         // Флаги событий (Команды, Таймер, DMA)
    osMessageQueueId_t  q_id;           // Очередь входящих команд
    osTimerId_t         scan_timer_id;  // Таймер кванта времени

    uint32_t            scan_step_ms;   // Величина одного "тика" (delta_ms)

    gpadc_manager_t* adc_manager;    // Ссылка на общего хозяина АЦП
    receiver_dsp_t      dsp;            // Встроенный блок анализа

    // Интерфейс приёмника (Привязка к конкретному железу)
    void* p_dev_data;     // Указатель на структуру тюнера (напр. rtc6715_t)

    // "Мозг" приёмника: вызывается на каждый тик таймера
    void (*process_state)(void* dev, uint32_t delta_ms);

    // Обработчик команд
    void (*handle_cmd)(void* dev, void* msg);

} receiver_worker_ctx_t;

/* Глобальные функции */
void receiver_worker_task(void *argument);
void receiver_dsp_perform_scan(receiver_worker_ctx_t *ctx);

#endif // RECEIVER_WORKER_H
