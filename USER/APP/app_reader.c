/** @copyright XunFang Communication Tech Limited. All rights reserved. 2015.
  * @file  app_reader.c
  * @author  netfire.liang
  * @version  V1.0.0
  * @date  07/06/2015
  * @brief  读卡器应用程序
  */ 
#define App_BSP_int_MODULE
#include <Define_All.h>
#include <App_Led.h>
#include "app_reader.h"
#include <module_reader.h>
#include <App_Debug.h>
#include <delay.h>
#include <algorithm.h>
#include <app_can.h>

#define DATA_ADDRESS_BALANCE 0x00
#define DATA_ADDRESS_POINT   0x04

typedef enum
{
    R_CMD_QUERY_TAGS,
    R_CMD_CREATE_TAGS,
    R_CMD_READ_UID, 
    R_CMD_READ_INFO,
    R_CMD_READ_DATA,
    R_CMD_WRITE_DATA,
    R_CMD_INIT_CARD,
    R_CMD_SET_MARK,
    R_CMD_DEDUCT,
    R_CMD_RECHARGE,
    R_CMD_READ_BALANCE,
    R_CMD_IS_TAG_ONLINE,
    R_CMD_SET_POWER,
    
} Reader_cmd_t;


typedef struct
{
    Reader_cmd_t cmd;
    UHF_Epc_t epc;
    int balance;
    uint8_t dst_address;
    uint8_t data_size;
    uint8_t data[64];
    
} Reader_data_t;

typedef struct
{
    Reader_module_t module;
    OS_EVENT *mbox;
    Reader_data_t sync_data;
    volatile uint8_t is_busy;
    
} App_reader_t;

extern void feed_watchdog(void);
static void Task_Reader(void *pdata);
static void _reader_unlock(App_reader_t *reader);
static uint8_t _reader_lock(App_reader_t *reader);
static int app_reader_test(App_reader_t *reader);
static void _process_cmd_query_tags(App_reader_t *reader);
static void _process_cmd_create_tags(App_reader_t *reader);
static void _process_cmd_read_uid(App_reader_t *reader);
static void _process_cmd_read_info(App_reader_t *reader);
static void _process_cmd_read_data(App_reader_t *reader);
static void _process_cmd_write_data(App_reader_t *reader);
static void _process_cmd_init_card(App_reader_t *reader);
static void _process_cmd_set_mark(App_reader_t *reader);
static void _process_cmd_deduct(App_reader_t *reader);
static void _process_cmd_recharge(App_reader_t *reader);
static void _process_cmd_read_balance(App_reader_t *reader);
static void _process_cmd_is_tag_online(App_reader_t *reader);

#define reader_max_number 4//4
static App_reader_t _reader[reader_max_number];

static uint8_t attach_reader = 0;

/**
  * @brief 创建读卡器的任务
  * @details 无
  * @param 无
  * @retval 无
 */
void create_reader_task(void)
{
    /* 初始化读卡器总线接口 */
    uhf_bus_initialize();
    
    /* 初始化读卡器数据 */
    for(int i = 0; i < reader_max_number; ++i)
    {
        char name[2] = {'A', '\0'};
        name[0] += i;
        Reader_module_t *reader = &_reader[i].module;
        
        reader->max_power = 33; /* 900M最大输出功率，写卡时用，单位dbm */
        reader->mid_power = 25; /* 掉线重新寻卡功率，单位dbm */
        reader->min_power = 25; /* 寻卡时用，单位dbm */
        reader->address = 0xff;/* 读卡器地址 */
        reader->antenna = i;
        
        if(!module_reader_create(reader, name))
        {
            debug_printf("Error: Reader exit!");
            return;
        }
        _reader[i].mbox = OSMboxCreate(0);
        assert_param(_reader[i].mbox);
        
        _reader_unlock(&_reader[i]);
    }
    uhf_set_beeper_mode(&_reader[0].module.device, 0);/* 蜂鸣器控制 */
    
    attach_reader = 1;
    
    OSTaskCreateExt(Task_Reader,
                    (void *)0,
                    &Stack_Task_Reader[StackSize_Task_Reader - 1],
                    (INT8U)Prio_Task_Reader,
                    (INT16U)Prio_Task_Reader,
                    &Stack_Task_Reader[0],
                    (INT32U)StackSize_Task_Reader,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
}

/**
  * @brief 读卡器主任务
  * @details 无
  * @param pdata 无用
  * @retval 无
 */
void Task_Reader(void *pdata)
{
    uint8_t index = 0;
    uint8_t err;
    App_reader_t *reader;
    
    debug_printf("[%s] enter\r\n", __func__);
    
    //app_reader_test(reader);
    while(1)
    {
        if(++index >= reader_max_number)
            index = 0;
        reader = &_reader[index];
        
        Reader_data_t *p = OSMboxPend(reader->mbox, 10, &err);
        if(p == NULL)
        {
            continue;
        }
        
        switch(p->cmd)
        {
        case R_CMD_QUERY_TAGS:
            _process_cmd_query_tags(reader);
            break;
       
        case R_CMD_CREATE_TAGS:
            _process_cmd_create_tags(reader);
            break;

        case R_CMD_READ_UID:
            _process_cmd_read_uid(reader);
            break;
        
        case R_CMD_READ_INFO:
            _process_cmd_read_info(reader);
            break;
        
        case R_CMD_READ_DATA:
            _process_cmd_read_data(reader);
            break;
        
        case R_CMD_WRITE_DATA:
            _process_cmd_write_data(reader);
            break;
            
        case R_CMD_INIT_CARD:
            _process_cmd_init_card(reader);
            break;
        
        case R_CMD_SET_MARK:
            _process_cmd_set_mark(reader);
            break;
            
       case R_CMD_DEDUCT:
            _process_cmd_deduct(reader);
            break;
       
       case R_CMD_RECHARGE:
            _process_cmd_recharge(reader);
            break;
       
        case R_CMD_READ_BALANCE:
            _process_cmd_read_balance(reader);
            break;
            
       case R_CMD_IS_TAG_ONLINE:
            _process_cmd_is_tag_online(reader);
            break;
       
        default:
            break;
        }
        
        _reader_unlock(reader);
        
    }/* loop */
}

static void _reader_unlock(App_reader_t *reader)
{
    OS_CPU_SR cpu_sr = 0u;
    
    OS_ENTER_CRITICAL();
    reader->is_busy = 0;
    OS_EXIT_CRITICAL();
}

static uint8_t _reader_lock(App_reader_t *reader)
{
    uint8_t last;
    
    OS_CPU_SR cpu_sr = 0u;
    OS_ENTER_CRITICAL();
    last = reader->is_busy;
    if(!last)
        reader->is_busy = 1;
    OS_EXIT_CRITICAL();
    
    return last;
}

static App_reader_t *_index_to_reader(uint8_t index)
{
    if(index >= reader_max_number)
        return NULL;
    
    App_reader_t *reader = &_reader[index];
    
    return reader;
}

static uint8_t reader_to_index(App_reader_t *reader)
{
    for(uint8_t i = 0; i < reader_max_number; ++i)
    {
        if(_index_to_reader(i) == reader)
            return i;
    }
    
    /* not reach here. */
    return 0xff;
}

uint8_t app_reader_is_attached(void)
{
    return attach_reader;    
}

int app_reader_query_tags(uint8_t dst_address, uint8_t reader_index, uint8_t max_tags)
{
    App_reader_t *reader = _index_to_reader(reader_index);
    
    if(reader == NULL)
        return -1;
    
    if(_reader_lock(reader))
        return 0;
    
    reader->sync_data.cmd = R_CMD_QUERY_TAGS;
    reader->sync_data.dst_address = dst_address;
    reader->sync_data.data[0] = max_tags;
    reader->sync_data.data_size = 1;
    OSMboxPost(reader->mbox, &reader->sync_data);

    return 1;
}

static void _process_cmd_query_tags(App_reader_t *reader)
{
    UHF_Epc_t epc[5];
    Canbus_ack_t ack = CANBUS_ACK_OK;
    uint8_t max_tags = reader->sync_data.data[0];
    uint8_t valid_epc_numbers = 0;
    
    if(max_tags > 5)
        max_tags = 5;
    
    valid_epc_numbers = module_reader_query_tags(&reader->module, epc, max_tags);
   
    if(!valid_epc_numbers)
    {
        ack = CANBUS_ACK_NO_TAG_ERROR;
        goto exit;
    }
    debug_printf("[%s] Query tags %d\r\n", reader->module.device.name, valid_epc_numbers);
    
exit:
    can_server_query_tags_notify(reader->sync_data.dst_address, reader_to_index(reader), ack, epc, valid_epc_numbers);
}

int app_reader_create_tags(uint8_t dst_address, uint8_t reader_index, const UHF_Epc_t *epc, int balance)
{
    App_reader_t *reader = _index_to_reader(reader_index);
    
    if(reader == NULL)
        return -1;
    
    if(_reader_lock(reader))
        return 0;
    
    reader->sync_data.epc = *epc;
    /* 金额 */
    put_uint32_t_value_to_buffer(balance, reader->sync_data.data);
    reader->sync_data.data_size = 4;
    
    reader->sync_data.cmd = R_CMD_CREATE_TAGS;
    OSMboxPost(reader->mbox, &reader->sync_data);

    return 1;
}

static void _process_cmd_create_tags(App_reader_t *reader)
{
    UHF_Epc_t epc[3];
    Canbus_ack_t ack = CANBUS_ACK_OK;
    int balance;
    
    uint8_t valid_epc_numbers = module_reader_query_tags(&reader->module, epc, 3);
    if(valid_epc_numbers == 0)
    {
        ack = CANBUS_ACK_NO_TAG_ERROR;
        goto exit;
    }
    else if(valid_epc_numbers != 1) /* 卡数量大于1张 */
    {
        ack = CANBUS_ACK_TAG_FULL_ERROR;
        goto exit;
    }

    if(module_reader_write_epc(&reader->module, &epc[0], &reader->sync_data.epc, 1) != 1)
    {
        ack = CANBUS_ACK_WRTE_EPC_ERROR;
        goto exit;
    }
    
    balance = get_uint32_value_from_buffer(reader->sync_data.data);
    if(module_reader_write_data(&reader->module, &reader->sync_data.epc, 5, DATA_ADDRESS_BALANCE, (uint8_t *)&balance, 4) != 1)
    {
        ack = CANBUS_ACK_WRITE_DATA_ERROR;
        goto exit;
    }
    
exit:
    can_server_create_tags_notify(reader->sync_data.dst_address, reader_to_index(reader), ack);
}

int app_reader_read_uid(uint8_t dst_address, uint8_t reader_index)
{
    App_reader_t *reader = _index_to_reader(reader_index);
    
    if(reader == NULL)
        return -1;
    
    if(_reader_lock(reader))
        return 0;
    
    reader->sync_data.cmd = R_CMD_READ_UID;
    OSMboxPost(reader->mbox, &reader->sync_data);

    return 1;
}

static void _process_cmd_read_uid(App_reader_t *reader)
{
    UHF_Epc_t epc;
    Canbus_ack_t ack = CANBUS_ACK_OK;
#define tid_buffer_size 8
    uint8_t tid_buffer[tid_buffer_size];
    
    uint8_t valid_epc_numbers = module_reader_query_tags(&reader->module, &epc, 1);
    if(!valid_epc_numbers)
    {
        ack = CANBUS_ACK_NO_TAG_ERROR;
        goto exit;
    }
    
    if(module_reader_read_tid(&reader->module, &epc, tid_buffer, tid_buffer_size) != 1)
    {
        ack = CANBUS_ACK_READ_TID_ERROR;
        goto exit;    
    }
    
exit:
    can_server_read_uid_notify(reader->sync_data.dst_address, reader_to_index(reader), ack, tid_buffer, tid_buffer_size);
}

int app_reader_read_info(uint8_t dst_address, uint8_t reader_index)
{
    App_reader_t *reader = _index_to_reader(reader_index);
    
    if(reader == NULL)
        return -1;
    
    if(_reader_lock(reader))
        return 0;
    
    reader->sync_data.cmd = R_CMD_READ_INFO;
    OSMboxPost(reader->mbox, &reader->sync_data);

    return 1;
}

static void _process_cmd_read_info(App_reader_t *reader)
{
    UHF_Epc_t epc;
    Canbus_ack_t ack = CANBUS_ACK_OK;
#define tid_buffer_size 8
    uint8_t tid_buffer[tid_buffer_size];
    int balance;
    
    uint8_t valid_epc_numbers = module_reader_query_tags(&reader->module, &epc, 1);
    if(!valid_epc_numbers)
    {
        ack = CANBUS_ACK_NO_TAG_ERROR;
        goto exit;
    }
    
    if(module_reader_read_tid(&reader->module, &epc, tid_buffer, tid_buffer_size) != 1)
    {
        ack = CANBUS_ACK_READ_TID_ERROR;
        goto exit;    
    }
    
    if(module_reader_read_data(&reader->module, &epc, 5, DATA_ADDRESS_BALANCE, (uint8_t *)&balance, 4) != 1)
    {
        ack = CANBUS_ACK_READ_DATA_ERROR;
        goto exit; 
    }
    
exit:
    can_server_read_info_notify(reader->sync_data.dst_address, reader_to_index(reader), ack, &epc, tid_buffer, balance);
}

int app_reader_read_data(uint8_t dst_address, uint8_t reader_index, const UHF_Epc_t *epc)
{
    App_reader_t *reader = _index_to_reader(reader_index);
    
    if(reader == NULL)
        return -1;
    
    if(_reader_lock(reader))
        return 0;
    
    reader->sync_data.cmd = R_CMD_READ_DATA;
    reader->sync_data.epc = *epc;
    reader->sync_data.data_size = 0;
    OSMboxPost(reader->mbox, &reader->sync_data);

    return 1;
}

static void _process_cmd_read_data(App_reader_t *reader)
{
    can_server_read_data_notify(reader->sync_data.dst_address, reader_to_index(reader), CANBUS_ACK_READ_DATA_ERROR, NULL, 0);
}

int app_reader_write_data(uint8_t dst_address, uint8_t reader_index, const UHF_Epc_t *epc, const uint8_t *buffer, uint8_t buffer_size)
{
    App_reader_t *reader = _index_to_reader(reader_index);
    
    if(reader == NULL)
        return -1;
    
    if(_reader_lock(reader))
        return 0;
    
    reader->sync_data.cmd = R_CMD_WRITE_DATA;
    reader->sync_data.epc = *epc;
    memcpy(reader->sync_data.data, buffer, buffer_size);
    reader->sync_data.data_size = buffer_size;
    OSMboxPost(reader->mbox, &reader->sync_data);
    
    return 1;
}

static void _process_cmd_write_data(App_reader_t *reader)
{
    can_server_write_data_notify(reader->sync_data.dst_address, reader_to_index(reader), CANBUS_ACK_WRITE_DATA_ERROR);
}

int app_reader_init_card(uint8_t dst_address, uint8_t reader_index)
{
    App_reader_t *reader = _index_to_reader(reader_index);
    
    if(reader == NULL)
        return -1;
    
    if(_reader_lock(reader))
        return 0;
    
    reader->sync_data.cmd = R_CMD_INIT_CARD;
    
    OSMboxPost(reader->mbox, &reader->sync_data);
    
    return 1;
}

static void _process_cmd_init_card(App_reader_t *reader)
{
    can_server_init_card_notify(reader->sync_data.dst_address, reader_to_index(reader), CANBUS_ACK_OK);
}

static int app_reader_test(App_reader_t *reader)
{
#define max_epc_number 10
    UHF_Epc_t epc[max_epc_number], origin_epc;
#define thiz_buffer_size 10
    uint8_t thiz_buffer[thiz_buffer_size];
    
    uint8_t valid_epc_numbers = module_reader_query_tags(&reader->module, &epc[0], max_epc_number);
    if(!valid_epc_numbers)
    {   
        return 0;
    }
    
    origin_epc = epc[0];
    epc[0].epc[0]++;
    
    if(module_reader_write_epc(&reader->module, &origin_epc, &epc[0], 0x01) != 1)
    {
        return -1;
    }
    
    if(module_reader_read_data(&reader->module, &epc[0], 0x01, 0x00, thiz_buffer, thiz_buffer_size) != 1)
    {
        return -2;
    }
    
    thiz_buffer[0]++;
    
    if(module_reader_write_data(&reader->module, &epc[0], 0x01, 0x00, thiz_buffer, thiz_buffer_size) != 1)
    {
        return -1;
    }
    
    return 1;
}

int app_reader_set_mark(uint8_t dst_address, uint8_t reader_index, UHF_Epc_t *epc, uint8_t mark)
{
    App_reader_t *reader = _index_to_reader(reader_index);
    
    if(reader == NULL)
        return -1;
    
    if(_reader_lock(reader))
        return 0;
    
    reader->sync_data.cmd = R_CMD_SET_MARK;
    reader->sync_data.epc = *epc;
    reader->sync_data.data[0] = mark;
    reader->sync_data.data_size = 1;
    OSMboxPost(reader->mbox, &reader->sync_data);
    
    return 1;
}

static void _process_cmd_set_mark(App_reader_t *reader)
{
    Canbus_ack_t ack = CANBUS_ACK_OK;
    uint16_t mark = reader->sync_data.data[0];
    
    debug_printf("[%s] set mark %d\r\n", reader->module.device.name, mark);
    if(module_reader_write_data(&reader->module, &reader->sync_data.epc, 10, DATA_ADDRESS_POINT, (uint8_t *)&mark, 0x02) != 1)
    {
        ack = CANBUS_ACK_WRITE_DATA_ERROR;
        goto exit;
    }
    
exit:
    can_server_set_mark_notify(reader->sync_data.dst_address, reader_to_index(reader), ack);
}

int app_reader_deduct(uint8_t dst_address, uint8_t reader_index, UHF_Epc_t *epc, uint8_t mark, int dec_balance)
{
    App_reader_t *reader = _index_to_reader(reader_index);
    
    if(reader == NULL)
        return -1;
    
    if(_reader_lock(reader))
        return 0;
    
    reader->sync_data.cmd = R_CMD_DEDUCT;
    reader->sync_data.epc = *epc;
    reader->sync_data.data[0] = mark;
    reader->sync_data.data_size = 1;
    reader->sync_data.balance = dec_balance;
    OSMboxPost(reader->mbox, &reader->sync_data);
    
    return 1;
}

static void _process_cmd_deduct(App_reader_t *reader)
{
    Canbus_ack_t ack = CANBUS_ACK_OK;
    uint16_t mark = reader->sync_data.data[0];
    int res_balance;
    int dec_balance = reader->sync_data.balance;
    
    /* 更新标志位 */
    if(module_reader_write_data(&reader->module, &reader->sync_data.epc, 5, DATA_ADDRESS_POINT, (uint8_t *)&mark, 2) != 1)
    {
        ack = CANBUS_ACK_WRITE_DATA_ERROR;
        goto exit;
    }
    
    /* 读余额 */
    if(module_reader_read_data(&reader->module, &reader->sync_data.epc, 5, DATA_ADDRESS_BALANCE, (uint8_t *)&res_balance, 4) != 1)
    {
        ack = CANBUS_ACK_WRITE_DATA_ERROR;
        goto exit;
    }

    if(res_balance < dec_balance)
    {
        ack = CANBUS_ACK_LESS_BALANCE_ERROR; /* 余额不足 */
        goto exit;
    }
    
    /* 扣费 */
    res_balance -= dec_balance;
    
    /* 写余额 */
    if(module_reader_write_data(&reader->module, &reader->sync_data.epc, 5, DATA_ADDRESS_BALANCE, (uint8_t *)&res_balance, 4) != 1)
    {
        ack = CANBUS_ACK_DEDUCT_BALANCE_ERROR;
        goto exit;
    }
    
exit:
    can_server_deduct_notify(reader->sync_data.dst_address, reader_to_index(reader), ack, res_balance);
}

int app_reader_recharge(uint8_t dst_address, uint8_t reader_index, UHF_Epc_t *epc, int charge_balance)
{
    App_reader_t *reader = _index_to_reader(reader_index);
    
    if(reader == NULL)
        return -1;
    
    if(_reader_lock(reader))
        return 0;
    
    reader->sync_data.cmd = R_CMD_RECHARGE;
    reader->sync_data.epc = *epc;
    reader->sync_data.balance = charge_balance;
    OSMboxPost(reader->mbox, &reader->sync_data);
    
    return 1;
}

static void _process_cmd_recharge(App_reader_t *reader)
{
    Canbus_ack_t ack = CANBUS_ACK_OK;
    int res_balance;
    int charge_balance = reader->sync_data.balance;
    
    /* 读余额 */
    if(module_reader_read_data(&reader->module, &reader->sync_data.epc, 5, DATA_ADDRESS_BALANCE, (uint8_t *)&res_balance, 4) != 1)
    {
        ack = CANBUS_ACK_WRITE_DATA_ERROR;
        goto exit;
    }

    /* 充值 */
    res_balance += charge_balance;
    
    /* 写余额 */
    if(module_reader_write_data(&reader->module, &reader->sync_data.epc, 5, DATA_ADDRESS_BALANCE, (uint8_t *)&res_balance, 4) != 1)
    {
        ack = CANBUS_ACK_DEDUCT_BALANCE_ERROR;
        goto exit;
    }
    
exit:
    can_server_recharge_notify(reader->sync_data.dst_address, reader_to_index(reader), ack, res_balance);
}

int app_reader_read_balance(uint8_t dst_address, uint8_t reader_index, UHF_Epc_t *epc)
{
    App_reader_t *reader = _index_to_reader(reader_index);
    
    if(reader == NULL)
        return -1;
    
    if(_reader_lock(reader))
        return 0;
    
    reader->sync_data.cmd = R_CMD_READ_BALANCE;
    reader->sync_data.epc = *epc;
    OSMboxPost(reader->mbox, &reader->sync_data);
    
    return 1;
}

static void _process_cmd_read_balance(App_reader_t *reader)
{
    Canbus_ack_t ack = CANBUS_ACK_OK;
    int res_balance;
    
    /* 读余额 */
    if(module_reader_read_data(&reader->module, &reader->sync_data.epc, 5, DATA_ADDRESS_BALANCE, (uint8_t *)&res_balance, 4) != 1)
    {
        ack = CANBUS_ACK_WRITE_DATA_ERROR;
        goto exit;
    }

exit:
    can_server_read_balance_notify(reader->sync_data.dst_address, reader_to_index(reader), ack, res_balance);
}

int app_reader_is_tag_online(uint8_t dst_address, uint8_t reader_index, UHF_Epc_t *epc, uint8_t try_counter)
{
    App_reader_t *reader = _index_to_reader(reader_index);
    
    if(reader == NULL)
        return -1;
    
    if(_reader_lock(reader))
        return 0;
    
    reader->sync_data.cmd = R_CMD_IS_TAG_ONLINE;
    reader->sync_data.epc = *epc;
    reader->sync_data.data[0] = try_counter;
    OSMboxPost(reader->mbox, &reader->sync_data);
    
    return 1;
}

static void _process_cmd_is_tag_online(App_reader_t *reader)
{
    Canbus_ack_t ack = CANBUS_ACK_OK;
    uint8_t try_counter = reader->sync_data.data[0];
    if(try_counter == 0)
        try_counter = 0x01;
    
    debug_printf("[%s] is tag online %d\r\n", reader->module.device.name);
    if(module_reader_is_online(&reader->module, &reader->sync_data.epc, try_counter) != 1)
    {
        ack = CANBUS_ACK_NO_TAG_ERROR;
        goto exit;
    }
    
exit:
    can_server_is_tag_online_notify(reader->sync_data.dst_address, reader_to_index(reader), ack);
}
//设置读卡功率
int app_reader_set_power(uint8_t dst_address, uint8_t reader_index, 
                         uint8_t query_tag_power, 
                         uint8_t read_tag_power, 
                         uint8_t wirte_tag_power)
{
    App_reader_t *reader = _index_to_reader(reader_index);
    
    if(reader == NULL)
        return -1;
    
    reader->module.min_power = query_tag_power;
    reader->module.mid_power = read_tag_power;
    reader->module.max_power = wirte_tag_power;
    
    return 1;
}
