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
                	receiver_base_cmd(&ctx->base, msg_ptr);
//                    if (ctx->handle_cmd) {
//                        ctx->handle_cmd(ctx->p_dev_data, msg_ptr);
//                    }
                }
            }

            // 2. ЛОГИКА АВТОМАТА (Активная часть)
            if (event & SIG_REC_TIMER_DONE) {
                // Просто передаем управление приёмнику и говорим, сколько времени прошло.
                // Приёмник сам решит: пора ли переключать PLL или пора звать DSP.
//                if (ctx->process_state) {
//                    ctx->process_state(ctx->p_dev_data, ctx->scan_step_ms);
//                }
            	receiver_process_state(&ctx->base, ctx->scan_step_ms);
            }
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint32_t receiver_worker_make_ctrl_cmd(receiver_worker_ctx_t *p_ctl,uint8_t cmd, uint8_t len, uint8_t *p_data){
cntrl_dev_sys_msg_que_type_s* p_buf;
cntrl_dev_sys_msg_que_type_s buf;
uint32_t res=0;
	p_buf = &buf;
	p_buf->cmd = cmd;
	p_buf->have_ack = dev_cntrl_ack_not_response;
	p_buf->time = 0;//osKernelGetTickCount();
	if(len){
		memcpy(&p_buf->buf[0],p_data,len);
	}
	if (osMessageQueuePut(p_ctl->q_id,p_buf,0, 1000) != osOK){
		res = 2;
	}
	else{
		osEventFlagsSet(p_ctl->evt_id,SIG_REC_NEW_CMD);
	}
	return res;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void receiver_worker_make_ctrl_cmd_32(uint32_t ctl32, uint32_t tmp0, uint32_t cmd, uint8_t* p8_data){
	receiver_worker_ctx_t *p_ctl;
	p_ctl = (receiver_worker_ctx_t *)ctl32;
	receiver_worker_make_ctrl_cmd(p_ctl,cmd,tmp0,p8_data);
	return;
}
