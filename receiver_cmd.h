/*
 * receiver_cmd.h
 * Заголовочный файл обработчика команд управления приёмниками
 */

#ifndef RECEIVER_CMD_H
#define RECEIVER_CMD_H

#include <stdint.h>
#include "receiver_base.h"
#include "debug_cells_lib.h"

/* ============================================================================
 * Публичные функции
 * ============================================================================ */

/**
 * @brief Обработчик команд приёмника (публичный интерфейс)
 * Эта функция назначается в receiver_worker_ctx_t.handle_cmd
 * 
 * @param dev_ptr Указатель на структуру приёмника (receiver_base_t)
 * @param msg_ptr Указатель на сообщение из очереди (cntrl_dev_sys_msg_que_type_s)
 */
void receiver_base_handle_cmd(void* dev_ptr, void* msg_ptr);

/**
 * @brief Добавить команды приёмника в таблицу отладочных ячеек
 * 
 * @param p_cell Массив ячеек SettingCell_t
 * @param num Начальный индекс в массиве
 * @param p_recv Указатель на структуру приёмника
 * @param p_name Префикс имени для отладки (например, "RX1.2" или "RX5.8")
 * @return Количество добавленных ячеек
 */
uint32_t receiver_cmd_add_to_cell_table(SettingCell_t *p_cell, uint32_t num, receiver_base_t *p_recv, const char *p_name);

/* ============================================================================
 * Отладочные функции (для использования в cmd_h_add_full_to_cell_table)
 * ============================================================================ */

/**
 * @brief Получить текущее название канала/частоты (для отладки)
 * Формат возвращаемой строки: "A1", "B5", "CH-10" или "5865 MHz"
 */
uint8_t* receiver_dbg_get_current_name(uint32_t recv_base_addr, uint32_t cmd, uint32_t tmp);

/**
 * @brief Получить текущий режим работы (для отладки)
 * Возвращает: "MANUAL", "AUTO_SCAN", "FREQ_SCANNER", "LOCKED_ONLY"
 */
uint8_t* receiver_dbg_get_work_mode(uint32_t recv_base_addr, uint32_t cmd, uint32_t tmp);

/**
 * @brief Получить текущее состояние автомата (для отладки)
 * Возвращает: "TUNE", "WAIT_PLL", "DSP_SCAN", "LOCKED"
 */
uint8_t* receiver_dbg_get_state(uint32_t recv_base_addr, uint32_t cmd, uint32_t tmp);

/**
 * @brief Получить RSSI последнего измерения (для отладки)
 * Возвращает строку с числовым значением магнитуды сигнала
 */
uint8_t* receiver_dbg_get_rssi(uint32_t recv_base_addr, uint32_t cmd, uint32_t tmp);

#endif /* RECEIVER_CMD_H */
