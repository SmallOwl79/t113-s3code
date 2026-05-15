/*
 * receiver_init.c
 * Инициализация потоков управления приёмниками (1.2G и 5.8G)
 * на базе архитектуры receiver_worker
 */

#include "string.h"
#include "control.h"
#include "receiver_worker.h"
#include "receiver_base.h"
#include "Device/Inc/rtc6715.h"
#include "Device/Inc/recv_1_2.h"

/* ============================================================================
 * Глобальные объекты синхронизации для общего ресурса АЦП
 * ============================================================================ */
#pragma location=".ddr_data"
static uint32_t gpadc_mutex_cb[osRtxMutexCbSize / 4];
static gpadc_manager_t gpadc_manager = {
    .mutex = NULL,
    .is_initialized = false
};

/* ============================================================================
 * Контексты воркеров для каждого приёмника
 * ============================================================================ */
#pragma location=".ddr_data"
static uint32_t worker_1_2_queue_mem[(16 * (sizeof(cntrl_dev_sys_msg_que_type_s) + 12)) / 4];
#pragma location=".ddr_data"
static uint32_t worker_1_2_queue_cb[osRtxMessageQueueCbSize / 4];
#pragma location=".ddr_data"
static uint32_t worker_1_2_evt_cb[osRtxEventFlagsCbSize / 4];

#pragma location=".ddr_data"
static uint32_t worker_5_8_queue_mem[(16 * (sizeof(cntrl_dev_sys_msg_que_type_s) + 12)) / 4];
#pragma location=".ddr_data"
static uint32_t worker_5_8_queue_cb[osRtxMessageQueueCbSize / 4];
#pragma location=".ddr_data"
static uint32_t worker_5_8_evt_cb[osRtxEventFlagsCbSize / 4];

/* Сами контексты воркеров */
#pragma location=".ddr_data"
static receiver_worker_ctx_t worker_1_2_ctx;
#pragma location=".ddr_data"
static receiver_worker_ctx_t worker_5_8_ctx;

/* Базовые экземпляры приёмников */
#pragma location=".ddr_data"
static receiver_base_t recv_1_2_base;
#pragma location=".ddr_data"
static receiver_base_t recv_5_8_base;

/* Профили конфигурации */
#pragma location=".ddr_data"
static receiver_profile_t profile_1_2;
#pragma location=".ddr_data"
static receiver_profile_t profile_5_8;

/* Потоки воркеров */
#pragma location=".ddr_data"
static uint32_t worker_1_2_stack[512];
#pragma location=".ddr_data"
static uint32_t worker_5_8_stack[512];

/* ============================================================================
 * Функции установки частоты для конкретных драйверов
 * ============================================================================ */
static void hw_set_freq_1_2(void* dev, uint16_t freq) {
#ifdef RECV_1_2
    recv_1_2_struct *p_recv = (recv_1_2_struct *)dev;
    recv_1_2_set_freq_up_auto(p_recv, freq);
#endif
}

static void hw_set_freq_5_8(void* dev, uint16_t freq) {
#ifdef RTC6715
    rtc6715_struct *p_recv = (rtc6715_struct *)dev;
    recv_5_8_set_freq_up_auto(p_recv, freq);
#endif
}

/* ============================================================================
 * Инициализация профиля приёмника 1.2G
 * ============================================================================ */
static void init_profile_1_2(receiver_profile_t *p_cfg) {
    p_cfg->p_desc = (recv_descriptor_t *)&grid_1_2_desc;
    p_cfg->total_ch = grid_1_2_desc.total_ch;
    
    // Разрешаем все каналы по умолчанию
    memset(p_cfg->allowed_mask, 0xFF, sizeof(p_cfg->allowed_mask));
    
    p_cfg->min_freq = 910;
    p_cfg->max_freq = 1680;
    p_cfg->auto_step_freq = 20;  // Шаг 20 МГц в автопоиске
    p_cfg->manual_step_freq = 10; // Шаг 10 МГц в ручном режиме
    p_cfg->scan_time_sec = 5;
    p_cfg->freq_ch_mode = RECV_MODE_CHANNEL;
}

/* ============================================================================
 * Инициализация профиля приёмника 5.8G
 * ============================================================================ */
static void init_profile_5_8(receiver_profile_t *p_cfg) {
#ifdef RECV_5_8_EXT_CH
    p_cfg->p_desc = (recv_descriptor_t *)&grid_5_8_ext_desc;
    p_cfg->total_ch = grid_5_8_ext_desc.total_ch;
#else
#ifdef RECV_5_8_BASE_CH
    p_cfg->p_desc = (recv_descriptor_t *)&grid_5_8_base_desc;
    p_cfg->total_ch = grid_5_8_base_desc.total_ch;
#else
    // По умолчанию используем расширенную сетку
    p_cfg->p_desc = (recv_descriptor_t *)&grid_5_8_ext_desc;
    p_cfg->total_ch = grid_5_8_ext_desc.total_ch;
#endif
#endif
    
    // Разрешаем все каналы по умолчанию
    memset(p_cfg->allowed_mask, 0xFF, sizeof(p_cfg->allowed_mask));
    
    p_cfg->min_freq = 4867;
    p_cfg->max_freq = 6184;
    p_cfg->auto_step_freq = 20;
    p_cfg->manual_step_freq = 10;
    p_cfg->scan_time_sec = 5;
    p_cfg->freq_ch_mode = RECV_MODE_CHANNEL;
}

/* ============================================================================
 * Инициализация воркера приёмника
 * ============================================================================ */
static void init_worker_ctx(receiver_worker_ctx_t *ctx, 
                           uint8_t id,
                           uint32_t *queue_mem,
                           uint32_t *queue_cb,
                           uint32_t *evt_cb,
                           receiver_dsp_t *p_dsp)
{
    const osMessageQueueAttr_t queue_attr = {
        .name = NULL,
        .cb_mem = queue_cb,
        .cb_size = sizeof(worker_1_2_queue_cb),
        .mq_mem = queue_mem,
        .mq_size = sizeof(worker_1_2_queue_mem)
    };
    
    const osEventFlagsAttr_t evt_attr = {
        .name = NULL,
        .cb_mem = evt_cb,
        .cb_size = sizeof(worker_1_2_evt_cb)
    };
    
    ctx->id = id;
    ctx->evt_id = osEventFlagsNew(&evt_attr);
    ctx->q_id = osMessageQueueNew(16, sizeof(cntrl_dev_sys_msg_que_type_s), &queue_attr);
    ctx->scan_step_ms = 50;  // Квант времени 50 мс
    ctx->adc_manager = &gpadc_manager;
    
    // Настройка DSP
    if (p_dsp) {
        p_dsp->n_samples = 256;      // Размер выборки АЦП
        p_dsp->n_repeats = 4;        // Количество усреднений
        p_dsp->target_bin = 16;      // Целевая частота (настроить под 15.6 кГц)
        p_dsp->threshold_mag = 0.5f; // Порог магнитуды
        p_dsp->threshold_snr = 3.0f; // Порог SNR
        p_dsp->is_video_found = false;
        
        // GPIO аналогового ключа (настроить под вашу плату)
        // p_dsp->sw_pin.port = GPIOA;
        // p_dsp->sw_pin.pin = GPIO_PIN_0;
        p_dsp->sw_state = id;  // 0 или 1 для разных тюнеров
    }
}

/* ============================================================================
 * Основная функция инициализации потоков приёмников
 * ============================================================================ */
void receiver_streams_init(control_struct *p_ctl) {
    const osThreadAttr_t thread_attr_1_2 = {
        .name = "RecvWorker1.2",
        .stack_mem = worker_1_2_stack,
        .stack_size = sizeof(worker_1_2_stack),
        .priority = osPriorityNormal
    };
    
    const osThreadAttr_t thread_attr_5_8 = {
        .name = "RecvWorker5.8",
        .stack_mem = worker_5_8_stack,
        .stack_size = sizeof(worker_5_8_stack),
        .priority = osPriorityNormal
    };
    
    /* 1. Инициализация менеджера АЦП (общий ресурс) */
    const osMutexAttr_t mutex_attr = {
        .name = "GPADC_Mutex",
        .cb_mem = gpadc_mutex_cb,
        .cb_size = sizeof(gpadc_mutex_cb)
    };
    gpadc_manager.mutex = osMutexNew(&mutex_attr);
    gpadc_manager.is_initialized = true;
    
    /* 2. Инициализация воркера 1.2G */
    init_worker_ctx(&worker_1_2_ctx, 
                    0, 
                    worker_1_2_queue_mem, 
                    worker_1_2_queue_cb, 
                    worker_1_2_evt_cb,
                    &worker_1_2_ctx.dsp);
    
    /* 3. Инициализация базового приёмника 1.2G */
    init_profile_1_2(&profile_1_2);
    recv_1_2_base.parent_worker_ctx = &worker_1_2_ctx;
    recv_1_2_base.bus_mutex = NULL;  // Можно добавить мьютекс для SPI/I2C
    recv_1_2_base.pll_lock_time_ms = 40;  // Время стабилизации PLL для 1.2G
    recv_1_2_base.hw_set_freq = hw_set_freq_1_2;
    recv_1_2_base.hw_dev_ptr = &p_ctl->recv_1_2;
    recv_1_2_base.p_cfg = &profile_1_2;
    recv_1_2_base.state = RECV_STATE_TUNE;
    recv_1_2_base.state_timer_ms = 0;
    recv_1_2_base.current_freq = 910;
    recv_1_2_base.current_ch_idx = 0;
    recv_1_2_base.work_mode = RECV_MODE_MANUAL;
    
    /* Привязка функций процессинга к воркеру */
    worker_1_2_ctx.p_dev_data = &recv_1_2_base;
    worker_1_2_ctx.process_state = receiver_process_state;
    worker_1_2_ctx.handle_cmd = receiver_base_handle_cmd;
    
    /* 4. Инициализация воркера 5.8G */
    init_worker_ctx(&worker_5_8_ctx, 
                    1, 
                    worker_5_8_queue_mem, 
                    worker_5_8_queue_cb, 
                    worker_5_8_evt_cb,
                    &worker_5_8_ctx.dsp);
    
    /* 5. Инициализация базового приёмника 5.8G */
    init_profile_5_8(&profile_5_8);
    recv_5_8_base.parent_worker_ctx = &worker_5_8_ctx;
    recv_5_8_base.bus_mutex = NULL;
    recv_5_8_base.pll_lock_time_ms = 10;  // Время стабилизации PLL для RTC6715
    recv_5_8_base.hw_set_freq = hw_set_freq_5_8;
    recv_5_8_base.hw_dev_ptr = &p_ctl->rtc6715;
    recv_5_8_base.p_cfg = &profile_5_8;
    recv_5_8_base.state = RECV_STATE_TUNE;
    recv_5_8_base.state_timer_ms = 0;
    recv_5_8_base.current_freq = 5865;
    recv_5_8_base.current_ch_idx = 0;
    recv_5_8_base.work_mode = RECV_MODE_MANUAL;
    
    /* Привязка функций процессинга к воркеру */
    worker_5_8_ctx.p_dev_data = &recv_5_8_base;
    worker_5_8_ctx.process_state = receiver_process_state;
    worker_5_8_ctx.handle_cmd = receiver_base_handle_cmd;
    
    /* 6. Запуск потоков воркеров */
#ifdef RECV_1_2
    osThreadNew(receiver_worker_task, &worker_1_2_ctx, &thread_attr_1_2);
#endif
    
#ifdef RTC6715
    osThreadNew(receiver_worker_task, &worker_5_8_ctx, &thread_attr_5_8);
#endif
}

/* ============================================================================
 * Функция отправки команды в поток приёмника
 * ============================================================================ */
uint32_t receiver_send_cmd(receiver_worker_ctx_t *ctx, 
                          receiver_cntrl_cmd_e cmd, 
                          uint16_t value)
{
    cntrl_dev_sys_msg_que_type_s msg;
    
    memset(&msg, 0, sizeof(msg));
    msg.cmd = (uint8_t)cmd;
    msg.uint16_val = value;
    
    if (osMessageQueuePut(ctx->q_id, &msg, 0, 100) != osOK) {
        return 1;  // Ошибка: очередь переполнена
    }
    
    osEventFlagsSet(ctx->evt_id, SIG_REC_NEW_CMD);
    return 0;
}

/* ============================================================================
 * Вспомогательные функции для отправки команд из основного кода
 * ============================================================================ */
#ifdef RECV_1_2
uint32_t recv_1_2_send_cmd(receiver_cntrl_cmd_e cmd, uint16_t value) {
    return receiver_send_cmd(&worker_1_2_ctx, cmd, value);
}
#endif

#ifdef RTC6715
uint32_t recv_5_8_send_cmd(receiver_cntrl_cmd_e cmd, uint16_t value) {
    return receiver_send_cmd(&worker_5_8_ctx, cmd, value);
}
#endif
