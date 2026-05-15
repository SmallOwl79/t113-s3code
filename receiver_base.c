#include "receiver_base.h"
#include "receiver_worker.h"

// ============================================================================
// ВАРИАНТ 1: RECV_5_8_BASE_CH (96 каналов, 12 литер)
// ============================================================================
static const uint16_t freq_5_8_base[] = {
    5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725, // A
    5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866, // B
    5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945, // E
    5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880, // F
    5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917, // R
    5362, 5399, 5436, 5473, 5510, 5547, 5584, 5621, // L
    5653, 5693, 5733, 5773, 5813, 5853, 5893, 5933, // H
    5333, 5373, 5413, 5453, 5493, 5533, 5573, 5613, // l
    5325, 5348, 5366, 5384, 5402, 5420, 5438, 5456, // U
    5474, 5492, 5510, 5528, 5546, 5564, 5582, 5600, // O
    4990, 5020, 5050, 5080, 5110, 5140, 5170, 5200, // X
    6002, 6028, 6054, 6080, 6106, 6132, 6158, 6184  // Z
};
static const char *lit_5_8_base[] = {"A","B","E","F","R","L","H","l","U","O","X","Z"};

const recv_descriptor_t grid_5_8_base_desc = {
    .freq_table   = freq_5_8_base,
    .total_ch     = 96,
    .ch_per_band  = 8,
    .band_names   = lit_5_8_base,
    .channel_names    = NULL
};

// ============================================================================
// ВАРИАНТ 2: RECV_5_8_SUB_3_3 + EXT_CH (56 каналов, 7 литер)
// ============================================================================
static const uint16_t freq_3_3_ext[] = {
    3200, 3220, 3240, 3260, 3280, 3300, 3320, 3340, // A
    3360, 3380, 3400, 3420, 3440, 3460, 3480, 3500, // B
    3520, 3540, 3560, 3580, 3600, 3620, 3640, 3680, // C
    3210, 3250, 3290, 3330, 3370, 3410, 3450, 3490, // D
    3230, 3290, 3350, 3410, 3470, 3530, 3590, 3700, // E
    3110, 3120, 3130, 3140, 3150, 3160, 3180, 3190, // F
    3710, 3720, 3730, 3740, 3750, 3760, 3780, 3790  // G
};
static const char *lit_3_3_ext[] = {"A","B","C","D","E","F","G"};

const recv_descriptor_t grid_3_3_ext_desc = {
    .freq_table   = freq_3_3_ext,
    .total_ch     = 56,
    .ch_per_band  = 8,
    .band_names   = lit_3_3_ext,
    .channel_names    = NULL,
};

// ============================================================================
// ВАРИАНТ 3: RECV_5_8_EXT_CH (Default / 120 каналов, 15 литер)
// ============================================================================
static const uint16_t freq_5_8_ext[] = {
    5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725, // A
    5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866, // B
    5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945, // E
    5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880, // F
    5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917, // R
    5210, 5230, 5250, 5270, 5300, 5330, 5350, 5370, // D
    5362, 5399, 5436, 5473, 5510, 5547, 5584, 5621, // L
    5653, 5693, 5733, 5773, 5813, 5853, 5893, 5933, // H
    5333, 5373, 5413, 5453, 5493, 5533, 5573, 5613, // l
    5325, 5348, 5366, 5384, 5402, 5420, 5438, 5456, // U
    5474, 5492, 5510, 5528, 5546, 5564, 5582, 5600, // O
    4990, 5020, 5050, 5080, 5110, 5140, 5170, 5200, // X
    4867, 4884, 4921, 4958, 4995, 5032, 5069, 5099, // J
    5960, 5980, 6000, 6020, 6040, 6060, 6080, 6100, // K
    6002, 6028, 6054, 6080, 6106, 6132, 6158, 6184  // Z
};
static const char *lit_5_8_ext[] = {"A","B","E","F","R","D","L","H","l","U","O","X","J","K","Z"};


static const uint16_t freq_1_2_table[] = {
    910,  980,  1010, 1040, 1080, 1120, 1160, 1200, 1240, 1280, // 0-9
    1320, 1380, 1360, 1258, 1100, 1140, 1405, 1430, 1455, 1480, // 10-19
    1505, 1530, 1555, 1580, 1605, 1630, 1640, 840,  860,  880   // 20-29
};

const recv_descriptor_t grid_1_2_desc = {
    .freq_table    = freq_1_2_table,
    .total_ch      = 30,             // Ровно 30 записей
    .ch_per_band   = 0,              // Плоский список, литер нет
    .band_names    = NULL,
    .channel_names = NULL            // Имена будут генерироваться как "CH-1" ... "CH-30"
};


const recv_descriptor_t grid_5_8_ext_desc = {
    .freq_table   = freq_5_8_ext,
    .total_ch     = 120,
    .ch_per_band  = 8,
    .band_names   = lit_5_8_ext,
    .channel_names    = NULL
};

receiver_profile_t cfg_sub = {
    .p_desc = (recv_descriptor_t*)&grid_1_2_desc,
    .min_freq = 800,
    .max_freq = 1700,
    .auto_step_freq = 10,
    .manual_step_freq = 10,
    .scan_time_sec = 5,
    .freq_ch_mode = RECV_MODE_CHANNEL
};

//receiver_profile_t prof_5_8_full = 		{ .p_grid = &grid_5_8_ext_desc };
//receiver_profile_t prof_1_2_default = 	{ .p_grid = &grid_1_2_desc };

/**
 * @brief Низкоуровневая функция захвата АЦП и анализа.
 * Вызывается изнутри process_state приёмника, когда тот готов к измерению.
 */
void receiver_dsp_perform_scan(receiver_base_t *ctx) {
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
//        uint32_t flags = osEventFlagsWait(ctx->evt_id, SIG_DMA_COMPLETE, osFlagsWaitAny, 100);
//        if (flags & osFlagsError) break;

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

void receiver_base_get_current_name(receiver_base_t *base, char *out_str) {
    recv_descriptor_t *d = base->p_cfg->p_desc;

    if (base->p_cfg->freq_ch_mode == RECV_MODE_FREQ) {
        // Если мы в частотном режиме — просто пишем частоту
        sprintf(out_str, "%d MHz", base->current_freq);
        return;
    }

    // Если есть специфичные имена для каждого канала — берем их
    if (d->channel_names != NULL) {
        strcpy(out_str, d->channel_names[base->current_ch_idx]);
        return;
    }

    // Стандартная логика литер (A1, B5 и т.д.)
    if (d->ch_per_band > 0 && d->band_names != NULL) {
        uint8_t band_idx = base->current_ch_idx / d->ch_per_band;
        uint8_t ch_in_band = (base->current_ch_idx % d->ch_per_band) + 1;

        sprintf(out_str, "%s%d", d->band_names[band_idx], ch_in_band);
    } else {
        // Если литер нет — просто порядковый номер
        sprintf(out_str, "CH-%d", base->current_ch_idx + 1);
    }
}

static uint16_t receiver_get_next_idx_universal(receiver_base_t *base,
                                               uint16_t step_size,
                                               nav_dir_e dir,
                                               nav_scope_e scope)
{
    const recv_descriptor_t *desc = base->p_cfg->p_desc;
    uint16_t total = desc->total_ch;
    uint16_t current = base->current_ch_idx;
    uint16_t ch_per_lit = desc->ch_per_band;

    // 1. Вычисляем границы поиска
    uint16_t min_idx = 0;
    uint16_t max_idx = total - 1;

    if (scope == NAV_SCOPE_LITER && ch_per_lit > 0) {
        // Ограничиваем поиск текущей литерой (например, 8..15)
        min_idx = (current / ch_per_lit) * ch_per_lit;
        max_idx = min_idx + ch_per_lit - 1;
    }

    // 2. Делаем первичный шаг
    int32_t next = (dir == NAV_DIR_UP) ? (current + step_size) : (current - step_size);

    // 3. Обработка выхода за границы (цикличность)
    // Важно: если scope == LITER, крутимся внутри литеры
    uint16_t range = max_idx - min_idx + 1;

    // Приводим к диапазону
    while (next > max_idx) next -= range;
    while (next < min_idx) next += range;

    uint16_t target = (uint16_t)next;

    // 4. Если попали на разрешенный канал — бинго!
    if (base->p_cfg->allowed_mask[target >> 5] & (1UL << (target & 31))) {
        return target;
    }

    // 5. Если ЗАПРЕЩЕН — ищем ближайший разрешенный в заданном направлении
    uint16_t search_idx = target;
    for (uint16_t i = 0; i < range; i++) {
        if (dir == NAV_DIR_UP) {
            search_idx++;
            if (search_idx > max_idx) search_idx = min_idx;
        } else {
            search_idx--;
            if (search_idx < min_idx) search_idx = max_idx;
        }

        if (base->p_cfg->allowed_mask[search_idx >> 5] & (1UL << (search_idx & 31))) {
            return search_idx;
        }
    }

    return current; // Если в литере всё запрещено — стоим на месте
}

/**
 * @brief Универсальный тикающий автомат приёмника (The Brain)
 * Вызывается диспетчером каждые X миллисекунд.
 */
void receiver_process_state(receiver_base_t* base, uint32_t delta_ms) {
//
//
//    // Родительский воркер нужен нам только для доступа к модулю DSP (АЦП)
//    receiver_worker_ctx_t *worker = (receiver_worker_ctx_t *)base->parent_worker_ctx;

    // 1. Накапливаем прошедшее время
    base->state_timer_ms += delta_ms;

    // 2. Логика автомата
    switch (base->state) {

        // ==========================================
        // СОСТОЯНИЕ 1: Установка новой частоты
        // ==========================================
        case RECV_STATE_TUNE: {
            uint16_t target_freq;

            // Определяем целевую частоту в зависимости от режима работы
            if (base->p_cfg->freq_ch_mode == RECV_MODE_CHANNEL) {
                // Канальный режим: берем частоту из таблицы по текущему индексу
                target_freq = base->p_cfg->p_desc->freq_table[base->current_ch_idx];
                base->current_freq = target_freq; // Синхронизируем для UI
            } else {
                // Частотный режим: работаем с частотой напрямую
                target_freq = base->current_freq;
            }
            
            // Если физический драйвер железа подключен
            if (base->hw_set_freq) {
                if (base->bus_mutex != NULL) {
                    osMutexAcquire(base->bus_mutex, osWaitForever);
                }
                base->hw_set_freq(base->hw_dev_ptr, target_freq);
                if (base->bus_mutex != NULL) {
                    osMutexRelease(base->bus_mutex);
                }
            }
            
            // Сбрасываем таймер и переходим к ожиданию лока PLL
            base->state_timer_ms = 0;
            base->state = RECV_STATE_WAIT_PLL;
            break;
        }

        // ==========================================
        // СОСТОЯНИЕ 2: Ожидание стабилизации железа
        // ==========================================
        case RECV_STATE_WAIT_PLL:
            // У каждого чипа своё время: RTC6715 ~10мс, синтезатор 1.2G ~40мс
            if (base->state_timer_ms >= base->pll_lock_time_ms) {

                // Железо готово. Запускаем "тяжелую" функцию захвата АЦП и анализа Гёрцелем.
                // Эта функция синхронная (ждет DMA), поэтому мы отдаем ей управление.
                receiver_dsp_perform_scan(base);

                // Анализ завершен, переходим к принятию решения
                base->state = RECV_STATE_DSP_SCAN;
            }
            break;

        // ==========================================
        // СОСТОЯНИЕ 3: Проверка результатов DSP
        // ==========================================
        case RECV_STATE_DSP_SCAN:
			if (base->dsp.is_video_found) {
				base->state = RECV_STATE_LOCKED;
			} else {
				// ВИДЕО НЕТ. Что делаем?
				if (base->work_mode == RECV_MODE_AUTO_SCAN) {
					// Раз мы в АВТО - прыгаем на следующий канал и в TUNE
					base->current_ch_idx = receiver_get_next_idx_universal(base, 1, NAV_DIR_UP, NAV_SCOPE_GLOBAL);
					base->state = RECV_STATE_TUNE;
				} else {
					// В ручном режиме просто остаемся здесь (или в IDLE)
					// base->state = RECV_STATE_TUNE; // Можно вернуться в TUNE, если нужно обновлять RSSI
				}
			}
			break;

        // ==========================================
        // СОСТОЯНИЕ 4: Удержание частоты (Lock)
        // ==========================================
        case RECV_STATE_LOCKED:
            // Периодически (например, раз в секунду) проверяем, не пропал ли сигнал
            if (base->state_timer_ms >= 1000) {

                receiver_dsp_perform_scan(base);

                if (!base->dsp.is_video_found) {
                    // Сигнал потерян! Сбрасываем таймер и запускаем поиск заново.
                    base->state_timer_ms = 0;
                    base->state = RECV_STATE_TUNE;
                } else {
                    // Сигнал на месте. Сбрасываем таймер и стоим дальше.
                    base->state_timer_ms = 0;
                }
            }
            break;

        default:
            // Защита автомата от неверного состояния
            base->state = RECV_STATE_TUNE;
            break;
    }
}
void receiver_base_cmd(receiver_base_t* base, void* msg_ptr) {

    cntrl_dev_sys_msg_que_type_s *msg;
    msg = (cntrl_dev_sys_msg_que_type_s *)msg_ptr;

    switch (msg->cmd) {
    // --- ГЛОБАЛЬНАЯ НАВИГАЦИЯ ---
            case receiver_cntrl_cmd_ch_up:
                base->work_mode = RECV_MODE_MANUAL;
                base->current_ch_idx = receiver_get_next_idx_universal(base, 1, NAV_DIR_UP, NAV_SCOPE_GLOBAL);
                base->state = RECV_STATE_TUNE;
                break;

            case receiver_cntrl_cmd_ch_dn:
                base->work_mode = RECV_MODE_MANUAL;
                base->current_ch_idx = receiver_get_next_idx_universal(base, 1, NAV_DIR_DN, NAV_SCOPE_GLOBAL);
                base->state = RECV_STATE_TUNE;
                break;

            // --- НАВИГАЦИЯ ПО ЛИТЕРАМ (Bands) ---
            case receiver_cntrl_cmd_lit_up:
                base->work_mode = RECV_MODE_MANUAL;
                base->current_ch_idx = receiver_get_next_idx_universal(base, base->p_cfg->p_desc->ch_per_band, NAV_DIR_UP, NAV_SCOPE_GLOBAL);
                base->state = RECV_STATE_TUNE;
                break;

            case receiver_cntrl_cmd_lit_dn:
                base->work_mode = RECV_MODE_MANUAL;
                base->current_ch_idx = receiver_get_next_idx_universal(base, base->p_cfg->p_desc->ch_per_band, NAV_DIR_DN, NAV_SCOPE_GLOBAL);
                base->state = RECV_STATE_TUNE;
                break;

            // --- УПРАВЛЕНИЕ РЕЖИМАМИ ---
            case receiver_cntrl_cmd_mode_auto_scan:
                base->work_mode = RECV_MODE_AUTO_SCAN;
                if (base->state == RECV_STATE_LOCKED) base->state = RECV_STATE_TUNE;
                break;

            case receiver_cntrl_cmd_mode_manual:
                base->work_mode = RECV_MODE_MANUAL;
                break;

//            // --- УСТАНОВКА ЗНАЧЕНИЙ ---
//            case receiver_cntrl_cmd_set_freq:
//                base->work_mode = RECV_MODE_MANUAL;
//                base->current_freq = msg->uint16_val; // MHz
//                base->p_cfg->freq_ch_mode = RECV_MODE_FREQ;
//                base->state = RECV_STATE_TUNE;
//                break;
//
//            case main_cntrl_cmd_set_chanel:
//                base->work_mode = RECV_MODE_MANUAL;
//                base->current_ch_idx = msg->uint16_val; // Index
//                base->p_cfg->freq_ch_mode = RECV_MODE_CHANNEL;
//                base->state = RECV_STATE_TUNE;
//                break;
    }
}
