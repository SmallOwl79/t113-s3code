#include "receiver_worker.h"
#include "mock.h" // Для кэш-функций и регистров

/**
 * @brief Колбэк таймера воркера.
 * Просто сообщает задаче, что прошел квант времени.
 */
static void rec_timer_callback(void *argument) {
    receiver_worker_ctx_t *ctx = (receiver_worker_ctx_t *)argument;
    osEventFlagsSet(ctx->evt_id, SIG_REC_TIMER_DONE);
}

/**
 * @brief Основная задача воркера (Диспетчер).
 * Не содержит логики задержек или перебора частот — только реакция на события.
 */
void receiver_worker_task(void *argument) {
    receiver_worker_ctx_t *ctx = (receiver_worker_ctx_t *)argument;
    uint32_t event;
    void *msg_ptr; // Тип сообщения зависит от вашей системы

    // Создаем периодический таймер (Tick)
    ctx->scan_timer_id = osTimerNew(rec_timer_callback, osTimerPeriodic, ctx, NULL);
    osTimerStart(ctx->scan_timer_id, ctx->scan_step_ms);

    for (;;) {
        // Ждем любое событие: Команда или Тик таймера
        event = osEventFlagsWait(ctx->evt_id, SIG_REC_NEW_CMD | SIG_REC_TIMER_DONE, osFlagsWaitAny, osWaitForever);

        if (!(event & 0x80000000)) {

            // 1. РАЗБОР КОМАНД (Реактивная часть)
            if (event & SIG_REC_NEW_CMD) {
                // Выгребаем все команды из очереди
                while (osMessageQueueGet(ctx->q_id, &msg_ptr, NULL, 0) == osOK) {
                    if (ctx->handle_cmd) {
                        ctx->handle_cmd(ctx->p_dev_data, msg_ptr);
                    }
                }
            }

            // 2. ЛОГИКА АВТОМАТА (Активная часть)
            if (event & SIG_REC_TIMER_DONE) {
                // Просто передаем управление приёмнику и говорим, сколько времени прошло.
                // Приёмник сам решит: пора ли переключать PLL или пора звать DSP.
                if (ctx->process_state) {
                    ctx->process_state(ctx->p_dev_data, ctx->scan_step_ms);
                }
            }
        }
    }
}

/**
 * @brief Низкоуровневая функция захвата АЦП и анализа.
 * Вызывается изнутри process_state приёмника, когда тот готов к измерению.
 */
void receiver_dsp_perform_scan(receiver_worker_ctx_t *ctx) {
    receiver_dsp_t *p_dsp = &ctx->dsp;
    float accum_mag = 0;
    float accum_noise = 0;

    // 1. Захватываем мьютекс АЦП (общий ресурс для всех воркеров)
    if (osMutexAcquire(ctx->adc_manager->mutex, osWaitForever) != osOK) return;

    // 2. Переключаем аналоговый ключ под мьютексом (атомарно)
    gpio_set_sun(&p_dsp->sw_pin, p_dsp->sw_state);

    // Короткая пауза (Settling time) для переходных процессов ключа
//    __asm volatile("nop; nop; nop; nop;");

    // 3. Цикл накопления результатов
    for (uint16_t i = 0; i < p_dsp->n_repeats; i++) {

        // Инвалидируем кэш перед работой DMA
        L1C_CleanInvalidateDCache_by_Addr((void *)p_dsp->adc_buffer, sizeof(p_dsp->adc_buffer));

        // Запуск аппаратного захвата (DMA + GPADC)
        // dma_capture_start(p_dsp->adc_buffer, p_dsp->n_samples);

        // Ждем сигнал завершения передачи от ISR DMA
        uint32_t flags = osEventFlagsWait(ctx->evt_id, SIG_DMA_COMPLETE, osFlagsWaitAny, 100);
        if (flags & osFlagsError) break;

        // Инвалидируем кэш после DMA, чтобы CPU читал данные из DDR
        L1C_InvalidateDCache_by_Addr((void *)p_dsp->adc_buffer, sizeof(p_dsp->adc_buffer));

        // Математика (Гёрцель)
        // float mag = goertzel_mag(p_dsp->adc_buffer, p_dsp->n_samples, p_dsp->target_bin);
        // accum_mag += mag;
        // ... (расчет шума и т.д.)
    }

    // 4. Финализация результатов в структуру воркера
    // p_dsp->last_mag = accum_mag / p_dsp->n_repeats;
    // p_dsp->is_video_found = ...

    // 5. Освобождаем ресурс для другого приёмника
    osMutexRelease(ctx->adc_manager->mutex);
}

