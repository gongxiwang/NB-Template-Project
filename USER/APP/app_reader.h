#ifndef _app_reader_h_include_
#define _app_reader_h_include_

#include <module_reader.h>

void create_reader_task(void);
uint8_t app_reader_is_attached(void);
int app_reader_query_tags(uint8_t dst_address, uint8_t reader_index, uint8_t max_tags);
int app_reader_write_epc_tags(uint8_t dst_address, uint8_t reader_index, const UHF_Epc_t *epc);
int app_reader_write_data(uint8_t dst_address, uint8_t reader_index, const UHF_Epc_t *epc, const uint8_t *buffer, uint8_t buffer_size);
int app_reader_create_tags(uint8_t dst_address, uint8_t reader_index, const UHF_Epc_t *epc, int balance);
int app_reader_read_uid(uint8_t dst_address, uint8_t reader_index);
int app_reader_read_info(uint8_t dst_address, uint8_t reader_index);
int app_reader_read_data(uint8_t dst_address, uint8_t reader_index, const UHF_Epc_t *epc);
int app_reader_init_card(uint8_t dst_address, uint8_t reader_index);
int app_reader_set_mark(uint8_t dst_address, uint8_t reader_index, UHF_Epc_t *epc, uint8_t mark);
int app_reader_deduct(uint8_t dst_address, uint8_t reader_index, UHF_Epc_t *epc, uint8_t mark, int dec_balance);
int app_reader_recharge(uint8_t dst_address, uint8_t reader_index, UHF_Epc_t *epc, int charge_balance);
int app_reader_read_balance(uint8_t dst_address, uint8_t reader_index, UHF_Epc_t *epc);
int app_reader_is_tag_online(uint8_t dst_address, uint8_t reader_index, UHF_Epc_t *epc, uint8_t try_counter);
int app_reader_set_power(uint8_t dst_address, uint8_t reader_index, 
                         uint8_t query_tag_power, 
                         uint8_t read_tag_power, 
                         uint8_t wirte_tag_power);

#endif /* _app_reader_h_include_ */
