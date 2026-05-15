/*
 * receiver_cmd.c
 * Обработчик команд управления приёмниками (1.2G и 5.8G)
 * Аналог povorot_cmd_parser для системы приёмников
 */

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "receiver_base.h"
#include "receiver_worker.h"
#include "receiver_init.h"
#include "control.h"
#include "debug_cells_lib.h"

/* ============================================================================
 * Статические функции-обработчики конкретных команд
 * ============================================================================ */

/**
 * @brief Обработчик команды "Канал вверх" (глобально)
 */
static void receiver_cmd_ch_up(receiver_base_t *p_recv, cntrl_dev_sys_msg_que_type_s *p_msg) {
    p_recv->work_mode = RECV_MODE_MANUAL;
    p_recv->current_ch_idx = receiver_get_next_idx_universal(p_recv, 1, NAV_DIR_UP, NAV_SCOPE_GLOBAL);
    p_recv->state = RECV_STATE_TUNE;
}

/**
 * @brief Обработчик команды "Канал вниз" (глобально)
 */
static void receiver_cmd_ch_dn(receiver_base_t *p_recv, cntrl_dev_sys_msg_que_type_s *p_msg) {
    p_recv->work_mode = RECV_MODE_MANUAL;
    p_recv->current_ch_idx = receiver_get_next_idx_universal(p_recv, 1, NAV_DIR_DN, NAV_SCOPE_GLOBAL);
    p_recv->state = RECV_STATE_TUNE;
}

/**
 * @brief Обработчик команды "Литера вверх"
 */
static void receiver_cmd_lit_up(receiver_base_t *p_recv, cntrl_dev_sys_msg_que_type_s *p_msg) {
    p_recv->work_mode = RECV_MODE_MANUAL;
    p_recv->current_ch_idx = receiver_get_next_idx_universal(p_recv, p_recv->p_cfg->p_desc->ch_per_band, NAV_DIR_UP, NAV_SCOPE_GLOBAL);
    p_recv->state = RECV_STATE_TUNE;
}

/**
 * @brief Обработчик команды "Литера вниз"
 */
static void receiver_cmd_lit_dn(receiver_base_t *p_recv, cntrl_dev_sys_msg_que_type_s *p_msg) {
    p_recv->work_mode = RECV_MODE_MANUAL;
    p_recv->current_ch_idx = receiver_get_next_idx_universal(p_recv, p_recv->p_cfg->p_desc->ch_per_band, NAV_DIR_DN, NAV_SCOPE_GLOBAL);
    p_recv->state = RECV_STATE_TUNE;
}

/**
 * @brief Обработчик команды "Канал вверх внутри литеры"
 */
static void receiver_cmd_ch_up_in_lit(receiver_base_t *p_recv, cntrl_dev_sys_msg_que_type_s *p_msg) {
    p_recv->work_mode = RECV_MODE_MANUAL;
    p_recv->current_ch_idx = receiver_get_next_idx_universal(p_recv, 1, NAV_DIR_UP, NAV_SCOPE_LITER);
    p_recv->state = RECV_STATE_TUNE;
}

/**
 * @brief Обработчик команды "Канал вниз внутри литеры"
 */
static void receiver_cmd_ch_dn_in_lit(receiver_base_t *p_recv, cntrl_dev_sys_msg_que_type_s *p_msg) {
    p_recv->work_mode = RECV_MODE_MANUAL;
    p_recv->current_ch_idx = receiver_get_next_idx_universal(p_recv, 1, NAV_DIR_DN, NAV_SCOPE_LITER);
    p_recv->state = RECV_STATE_TUNE;
}

/**
 * @brief Обработчик команды "Установить частоту"
 */
static void receiver_cmd_set_freq(receiver_base_t *p_recv, cntrl_dev_sys_msg_que_type_s *p_msg) {
    p_recv->work_mode = RECV_MODE_MANUAL;
    p_recv->current_freq = p_msg->uint16_val; // MHz
    p_recv->p_cfg->freq_ch_mode = RECV_MODE_FREQ;
    p_recv->state = RECV_STATE_TUNE;
}

/**
 * @brief Обработчик команды "Установить индекс канала"
 */
static void receiver_cmd_set_chan_idx(receiver_base_t *p_recv, cntrl_dev_sys_msg_que_type_s *p_msg) {
    p_recv->work_mode = RECV_MODE_MANUAL;
    p_recv->current_ch_idx = p_msg->uint16_val; // Index
    p_recv->p_cfg->freq_ch_mode = RECV_MODE_CHANNEL;
    p_recv->state = RECV_STATE_TUNE;
}

/**
 * @brief Обработчик команды "Режим автопоиска"
 */
static void receiver_cmd_mode_auto_scan(receiver_base_t *p_recv, cntrl_dev_sys_msg_que_type_s *p_msg) {
    p_recv->work_mode = RECV_MODE_AUTO_SCAN;
    if (p_recv->state == RECV_STATE_LOCKED) {
        p_recv->state = RECV_STATE_TUNE;
    }
}

/**
 * @brief Обработчик команды "Ручной режим"
 */
static void receiver_cmd_mode_manual(receiver_base_t *p_recv, cntrl_dev_sys_msg_que_type_s *p_msg) {
    p_recv->work_mode = RECV_MODE_MANUAL;
}

/**
 * @brief Обработчик команды "Установить режим отображения"
 */
static void receiver_cmd_set_view_mode(receiver_base_t *p_recv, cntrl_dev_sys_msg_que_type_s *p_msg) {
    // TODO: Реализовать установку режима отображения (инверсия, нормальный и т.д.)
    // Пока заглушка
    (void)p_recv;
    (void)p_msg;
}

/* ============================================================================
 * Таблица обработчиков команд
 * ============================================================================ */

/**
 * @brief Парсер команд приёмника
 * Вызывается из воркера при получении сообщения из очереди
 */
static void receiver_cmd_parser(receiver_base_t *p_recv, cntrl_dev_sys_msg_que_type_s *p_msg) {
    switch (p_msg->cmd) {
        case receiver_cntrl_cmd_ch_up:
            receiver_cmd_ch_up(p_recv, p_msg);
            break;
            
        case receiver_cntrl_cmd_ch_dn:
            receiver_cmd_ch_dn(p_recv, p_msg);
            break;
            
        case receiver_cntrl_cmd_lit_up:
            receiver_cmd_lit_up(p_recv, p_msg);
            break;
            
        case receiver_cntrl_cmd_lit_dn:
            receiver_cmd_lit_dn(p_recv, p_msg);
            break;
            
        case receiver_cntrl_cmd_ch_up_in_lit:
            receiver_cmd_ch_up_in_lit(p_recv, p_msg);
            break;
            
        case receiver_cntrl_cmd_ch_dn_in_lit:
            receiver_cmd_ch_dn_in_lit(p_recv, p_msg);
            break;
            
        case receiver_cntrl_cmd_set_freq:
            receiver_cmd_set_freq(p_recv, p_msg);
            break;
            
        case receiver_cntrl_cmd_set_chan_idx:
            receiver_cmd_set_chan_idx(p_recv, p_msg);
            break;
            
        case receiver_cntrl_cmd_mode_auto_scan:
            receiver_cmd_mode_auto_scan(p_recv, p_msg);
            break;
            
        case receiver_cntrl_cmd_mode_manual:
            receiver_cmd_mode_manual(p_recv, p_msg);
            break;
            
        case receiver_cntrl_cmd_set_view_mode:
            receiver_cmd_set_view_mode(p_recv, p_msg);
            break;
            
        default:
            // Неизвестная команда
            break;
    }
}

/* ============================================================================
 * Публичная функция обработки команд (вызывается из receiver_worker)
 * ============================================================================ */

/**
 * @brief Обработчик команд приёмника (публичный интерфейс)
 * Эта функция назначается в receiver_worker_ctx_t.handle_cmd
 */
void receiver_base_handle_cmd(void* dev_ptr, void* msg_ptr) {
    receiver_base_t *base = (receiver_base_t *)dev_ptr;
    cntrl_dev_sys_msg_que_type_s *msg = (cntrl_dev_sys_msg_que_type_s *)msg_ptr;
    
    // Вызываем парсер команд
    receiver_cmd_parser(base, msg);
}

/* ============================================================================
 * Функции отладки и отладочные строки
 * ============================================================================ */

/**
 * @brief Получить текущее название канала/частоты (для отладки)
 */
uint8_t* receiver_dbg_get_current_name(uint32_t recv_base_addr, uint32_t cmd, uint32_t tmp) {
    static char dbg_str[32];
    receiver_base_t *p_recv = (receiver_base_t*)recv_base_addr;
    
    receiver_base_get_current_name(p_recv, dbg_str);
    
    return (uint8_t*)dbg_str;
}

/**
 * @brief Получить текущий режим работы (для отладки)
 */
uint8_t* receiver_dbg_get_work_mode(uint32_t recv_base_addr, uint32_t cmd, uint32_t tmp) {
    static const char* mode_names[] = {
        "MANUAL",
        "AUTO_SCAN",
        "FREQ_SCANNER",
        "LOCKED_ONLY"
    };
    
    receiver_base_t *p_recv = (receiver_base_t*)recv_base_addr;
    
    if (p_recv->work_mode < sizeof(mode_names)/sizeof(mode_names[0])) {
        return (uint8_t*)mode_names[p_recv->work_mode];
    }
    
    return (uint8_t*)"UNKNOWN";
}

/**
 * @brief Получить текущее состояние автомата (для отладки)
 */
uint8_t* receiver_dbg_get_state(uint32_t recv_base_addr, uint32_t cmd, uint32_t tmp) {
    static const char* state_names[] = {
        "TUNE",
        "WAIT_PLL",
        "DSP_SCAN",
        "LOCKED"
    };
    
    receiver_base_t *p_recv = (receiver_base_t*)recv_base_addr;
    
    if (p_recv->state < sizeof(state_names)/sizeof(state_names[0])) {
        return (uint8_t*)state_names[p_recv->state];
    }
    
    return (uint8_t*)"UNKNOWN";
}

/**
 * @brief Получить RSSI последнего измерения (для отладки)
 */
uint8_t* receiver_dbg_get_rssi(uint32_t recv_base_addr, uint32_t cmd, uint32_t tmp) {
    static char dbg_str[16];
    receiver_base_t *p_recv = (receiver_base_t*)recv_base_addr;
    receiver_worker_ctx_t *worker = (receiver_worker_ctx_t*)p_recv->parent_worker_ctx;
    
    sprintf(dbg_str, "%.2f", worker->dsp.last_mag);
    
    return (uint8_t*)dbg_str;
}

/* ============================================================================
 * Добавление команд в таблицу отладочных ячеек
 * ============================================================================ */

/**
 * @brief Добавить команды приёмника в таблицу отладочных ячеек
 * @param p_cell Массив ячеек
 * @param num Начальный номер ячейки
 * @param p_recv Указатель на структуру приёмника
 * @param p_name Префикс имени
 * @return Количество добавленных ячеек
 */
uint32_t receiver_cmd_add_to_cell_table(SettingCell_t *p_cell, uint32_t num, receiver_base_t *p_recv, const char *p_name) {
    uint32_t count = 0;
    
    // Команды навигации
    cmd_h_add_to_cell_table_ext_size(&p_cell[num + count++], 
                                      receiver_cntrl_cmd_ch_up, 
                                      (void*)p_recv, 
                                      NULL, 
                                      NULL, 
                                      0, 
                                      FixIntegerCellType, 
                                      "%s_CH_UP", p_name);
    
    cmd_h_add_to_cell_table_ext_size(&p_cell[num + count++], 
                                      receiver_cntrl_cmd_ch_dn, 
                                      (void*)p_recv, 
                                      NULL, 
                                      NULL, 
                                      0, 
                                      FixIntegerCellType, 
                                      "%s_CH_DN", p_name);
    
    cmd_h_add_to_cell_table_ext_size(&p_cell[num + count++], 
                                      receiver_cntrl_cmd_lit_up, 
                                      (void*)p_recv, 
                                      NULL, 
                                      NULL, 
                                      0, 
                                      FixIntegerCellType, 
                                      "%s_LIT_UP", p_name);
    
    cmd_h_add_to_cell_table_ext_size(&p_cell[num + count++], 
                                      receiver_cntrl_cmd_lit_dn, 
                                      (void*)p_recv, 
                                      NULL, 
                                      NULL, 
                                      0, 
                                      FixIntegerCellType, 
                                      "%s_LIT_DN", p_name);
    
    cmd_h_add_to_cell_table_ext_size(&p_cell[num + count++], 
                                      receiver_cntrl_cmd_ch_up_in_lit, 
                                      (void*)p_recv, 
                                      NULL, 
                                      NULL, 
                                      0, 
                                      FixIntegerCellType, 
                                      "%s_CH_UP_IN_LIT", p_name);
    
    cmd_h_add_to_cell_table_ext_size(&p_cell[num + count++], 
                                      receiver_cntrl_cmd_ch_dn_in_lit, 
                                      (void*)p_recv, 
                                      NULL, 
                                      NULL, 
                                      0, 
                                      FixIntegerCellType, 
                                      "%s_CH_DN_IN_LIT", p_name);
    
    // Команды установки значений
    cmd_h_add_to_cell_table_ext_size(&p_cell[num + count++], 
                                      receiver_cntrl_cmd_set_freq, 
                                      (void*)p_recv, 
                                      NULL, 
                                      NULL, 
                                      0, 
                                      FixIntegerCellType, 
                                      "%s_SET_FREQ", p_name);
    
    cmd_h_add_to_cell_table_ext_size(&p_cell[num + count++], 
                                      receiver_cntrl_cmd_set_chan_idx, 
                                      (void*)p_recv, 
                                      NULL, 
                                      NULL, 
                                      0, 
                                      FixIntegerCellType, 
                                      "%s_SET_CHAN", p_name);
    
    // Команды режимов
    cmd_h_add_to_cell_table_ext_size(&p_cell[num + count++], 
                                      receiver_cntrl_cmd_mode_auto_scan, 
                                      (void*)p_recv, 
                                      NULL, 
                                      NULL, 
                                      0, 
                                      FixIntegerCellType, 
                                      "%s_AUTO_SCAN", p_name);
    
    cmd_h_add_to_cell_table_ext_size(&p_cell[num + count++], 
                                      receiver_cntrl_cmd_mode_manual, 
                                      (void*)p_recv, 
                                      NULL, 
                                      NULL, 
                                      0, 
                                      FixIntegerCellType, 
                                      "%s_MANUAL", p_name);
    
    // Отладочные переменные
    cmd_h_add_to_cell_table_ext_size(&p_cell[num + count++], 
                                      0xFF00 | 0x01,  // Специальный код для отладки
                                      (void*)p_recv, 
                                      (void*)receiver_dbg_get_current_name, 
                                      NULL, 
                                      RD_Att, 
                                      StringCellType, 
                                      "%s_CUR_NAME", p_name);
    
    cmd_h_add_to_cell_table_ext_size(&p_cell[num + count++], 
                                      0xFF00 | 0x02, 
                                      (void*)p_recv, 
                                      (void*)receiver_dbg_get_work_mode, 
                                      NULL, 
                                      RD_Att, 
                                      StringCellType, 
                                      "%s_WORK_MODE", p_name);
    
    cmd_h_add_to_cell_table_ext_size(&p_cell[num + count++], 
                                      0xFF00 | 0x03, 
                                      (void*)p_recv, 
                                      (void*)receiver_dbg_get_state, 
                                      NULL, 
                                      RD_Att, 
                                      StringCellType, 
                                      "%s_STATE", p_name);
    
    cmd_h_add_to_cell_table_ext_size(&p_cell[num + count++], 
                                      0xFF00 | 0x04, 
                                      (void*)p_recv, 
                                      (void*)receiver_dbg_get_rssi, 
                                      NULL, 
                                      RD_Att, 
                                      StringCellType, 
                                      "%s_RSSI", p_name);
    
    return count;
}
