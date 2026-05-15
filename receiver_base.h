#ifndef RECEIVER_BASE_H
#define RECEIVER_BASE_H

#include <stdint.h>
#include <stdbool.h>
#include "cmsis_os2.h"
#include "comm.h"
#include "receiver_worker.h"
// Подключаем твою структуру команд
// #include "control.h"

// Режимы сканирования
typedef enum {
    RECV_MODE_CHANNEL = 0,
    RECV_MODE_FREQ    = 1
} recv_freq_ch_mode_e;

typedef enum {
    RECV_STATE_TUNE = 0,
    RECV_STATE_WAIT_PLL,
    RECV_STATE_DSP_SCAN,
    RECV_STATE_LOCKED
} receiver_state_e;

typedef enum {
    NAV_DIR_UP,
    NAV_DIR_DN
} nav_dir_e;

typedef enum {
    NAV_SCOPE_GLOBAL,
    NAV_SCOPE_LITER  // Только внутри текущей 8-ки (A1..A8)
} nav_scope_e;

typedef enum {
    RECV_MODE_MANUAL = 0,    // Ручной режим: стоим на месте, никуда не переключаемся
    RECV_MODE_AUTO_SCAN,     // Автопоиск: бежим по сетке, пока не поймаем видео (is_video_found)
    RECV_MODE_FREQ_SCANNER,  // Режим сканера: быстро пробегаем диапазон для построения графика RSSI
    RECV_MODE_LOCKED_ONLY    // Режим удержания: если видео пропало, не уходим в поиск, а ждем здесь
} receiver_work_mode_e;

typedef enum {
    receiver_cntrl_cmd_none = 0,

    // Навигация
    receiver_cntrl_cmd_ch_up,           // Канал +1 (глобально по сетке)
    receiver_cntrl_cmd_ch_dn,           // Канал -1 (глобально по сетке)
    receiver_cntrl_cmd_lit_up,          // Литера +1 (прыжок на ch_per_band)
    receiver_cntrl_cmd_lit_dn,          // Литера -1 (прыжок назад на ch_per_band)
    receiver_cntrl_cmd_ch_up_in_lit,    // Канал +1 (циклично внутри литеры)
    receiver_cntrl_cmd_ch_dn_in_lit,    // Канал -1 (циклично внутри литеры)

    // Установка значений
    receiver_cntrl_cmd_set_freq,        // Установить частоту (uint16_t MHz)
    receiver_cntrl_cmd_set_chan_idx,    // Установить индекс канала (uint16_t index)

    // Режимы (наш новый Enum режимов)
    receiver_cntrl_cmd_mode_manual,     // Перейти в ручной режим
    receiver_cntrl_cmd_mode_auto_scan,  // Включить автопоиск видео

    // Настройки на лету
    receiver_cntrl_cmd_set_view_mode,   // Нормальный/Инверсия/И т.д.
} receiver_cntrl_cmd_e;

typedef struct {
    const uint16_t *freq_table;    // Таблица частот
    uint16_t total_ch;             // Общее кол-во
    uint8_t  ch_per_band;          // Кол-во каналов в литере (8 для 5.8G, 0 если литер нет)

    // Имена литер (например, {"A", "B", "E", "F", "R"})
    const char **band_names;

    // Если нужно специфичное именование для каждого канала (редко, но бывает)
    // Если NULL — используем стандартное "Band" + "Index" (A1, A2...)
    const char **channel_names;
} recv_descriptor_t;


// Общая конфигурация (Профиль) — один на диапазон (например, на все 5.8G)
typedef struct {
	recv_descriptor_t* p_desc;
//    const uint16_t *freq_table;  // <--- Сетка частот живет здесь!
    uint16_t total_ch;           // <--- Размер сетки
    uint32_t allowed_mask[8];    // <--- Маска разрешенных каналов

    uint16_t min_freq;
    uint16_t max_freq;
    uint16_t auto_step_freq;
    uint16_t manual_step_freq;
    uint8_t  scan_time_sec;
    recv_freq_ch_mode_e freq_ch_mode;
} receiver_profile_t;

// Базовый класс (Инстанс)
typedef struct receiver_base_s {
    void* parent_worker_ctx;    // Воркер нужен ТОЛЬКО чтобы дергать DSP
    osMutexId_t bus_mutex;
    
    uint32_t pll_lock_time_ms;
    void (*hw_set_freq)(void* dev, uint16_t freq);
    void* hw_dev_ptr;

    receiver_profile_t *p_cfg;  // Ссылка на настройки и сетки

    // --- Внутреннее состояние автомата ---
    receiver_state_e state;
    uint32_t         state_timer_ms;

    uint16_t         current_freq;   // Для UI и freq_mode
    uint16_t         current_ch_idx; // <--- Индекс канала живет ЗДЕСЬ!

    receiver_work_mode_e work_mode; // Текущий активный режим

} receiver_base_t;
// Глобальные функции
void receiver_process_state(void* dev_ptr, uint32_t delta_ms);
void receiver_base_handle_cmd(void* dev_ptr, void* msg_ptr); // <--- Разбор команд
void receiver_base_get_current_name(receiver_base_t *base, char *out_str);

// Функция навигации (нужна для receiver_cmd.c)
uint16_t receiver_get_next_idx_universal(receiver_base_t *base, uint16_t step_size, nav_dir_e dir, nav_scope_e scope);

#endif // RECEIVER_BASE_H
