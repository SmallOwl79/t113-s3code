/*
 * receiver_init.h
 * Заголовочный файл инициализации потоков приёмников
 */

#ifndef RECEIVER_INIT_H
#define RECEIVER_INIT_H

#include "control.h"
#include "receiver_worker.h"
#include "receiver_base.h"

/**
 * @brief Инициализация потоков управления приёмниками (1.2G и 5.8G)
 * 
 * Создает два потока воркеров:
 * - RecvWorker1.2 для приёмника 1.2G
 * - RecvWorker5.8 для приёмника 5.8G (RTC6715)
 * 
 * Каждый поток имеет:
 * - Очередь команд
 * - Флаги событий
 * - Таймер квантования времени
 * - Доступ к общему менеджеру АЦП через мьютекс
 * 
 * @param p_ctl Указатель на основную структуру управления
 */
void receiver_streams_init(control_struct *p_ctl);

/**
 * @brief Отправка команды в поток приёмника
 * 
 * @param ctx Контекст воркера
 * @param cmd Код команды (из receiver_cntrl_cmd_e)
 * @param value Дополнительное значение (частота, индекс канала и т.д.)
 * @return 0 если успешно, 1 если ошибка (очередь переполнена)
 */
uint32_t receiver_send_cmd(receiver_worker_ctx_t *ctx, 
                          receiver_cntrl_cmd_e cmd, 
                          uint16_t value);

#ifdef RECV_1_2
/**
 * @brief Отправка команды приёмнику 1.2G
 */
uint32_t recv_1_2_send_cmd(receiver_cntrl_cmd_e cmd, uint16_t value);
#endif

#ifdef RTC6715
/**
 * @brief Отправка команды приёмнику 5.8G (RTC6715)
 */
uint32_t recv_5_8_send_cmd(receiver_cntrl_cmd_e cmd, uint16_t value);
#endif

#endif // RECEIVER_INIT_H
