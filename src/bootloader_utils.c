#include "bootloader_utils.h"


IMAGELOAD_STATE imageload_state = IMAGELOAD_NOT_ACTIVE;
uint8_t         loader_image_buf[200000];
uint32_t        loader_image_buf_i = 0;
volatile int    load_proc_num = 0;
uint32_t        ptr;
uint16_t        mem_req_amount;



void print_msg(J1939_ID_t j_id, uint8_t* payload, uint8_t DLC){
    xprintf("sa: %02X ", j_id.SA);
    xprintf("da: %02X ", j_id.PS);
    xprintf("pf: %02X ", j_id.PF);
    xprintf("   payload: ");
    for(int i = 0; i <DLC; ++i)
        xprintf(" %02X", payload[i]);
    xprintf("\n");
}
void push_packet_to_another_can(uint32_t ext_id, uint8_t* data, uint8_t DLC, can_type* HANDLER_CAN){
    push_packet_to_can(ext_id, data, DLC, HANDLER_CAN);
}
void push_packet_to_can(uint32_t ext_id, uint8_t* data, uint8_t DLC, can_type* CANX){
    xprintf("out: ");
    print_msg(*(J1939_ID_t*)&ext_id, data, DLC);

    uint8_t transmit_mailbox;
    can_tx_message_type tx_msg_struct;

    tx_msg_struct.standard_id = 0;
    tx_msg_struct.extended_id = ext_id;

    tx_msg_struct.id_type = CAN_ID_EXTENDED;
    tx_msg_struct.frame_type = CAN_TFT_DATA;
    tx_msg_struct.dlc = DLC;

    memcpy(tx_msg_struct.data, data, DLC * sizeof(uint8_t));

    transmit_mailbox = can_message_transmit(CANX, &tx_msg_struct);

    while(can_transmit_status_get(CANX, (can_tx_mailbox_num_type)transmit_mailbox) != CAN_TX_STATUS_SUCCESSFUL);
}









J81_addr_claim_pkg_t addr_claim_pkg;
void J81_formur_this_addr(){
    uint32_t iden_num   = 2042; // random value set by manuf
    uint16_t manuf_code = 2042; // random value our comp is not reg

    addr_claim_pkg.iden_num_lsb = iden_num;
    addr_claim_pkg.iden_num_mid = (iden_num >> 8);
    addr_claim_pkg.iden_num_msb = (iden_num >> 16); 
    addr_claim_pkg.manuf_code_lsb = (manuf_code); 
    addr_claim_pkg.manuf_code_msb = (manuf_code);
    addr_claim_pkg.ecu_inst = 0;
    addr_claim_pkg.fun_inst = 0;
    addr_claim_pkg.fun = 129; //Off-board diagnostic-service tool (B12)
    addr_claim_pkg.vehicle_sys = 0; // non specific sys
    addr_claim_pkg.vehicle_sys_inst = 0; //offboard diafnostic service tool in most cases is 0
    addr_claim_pkg.industry_group = AGRICULTURE_AND_FORESTRY_EQUIPMENT;
    addr_claim_pkg.arb_addr_cab = 0; //J81 str 9 we here not fot resolve other addrs conflicts
}
void addr_claim(){
    J81_formur_this_addr();
    J1939_ID_t addr_claim_id[1];
    formur_addr_claim_id(addr_claim_id, 0x83);
    if(get_mcu_num() == 4) push_packet_to_can(addr_claim_id->allmem, (uint8_t*)&addr_claim_pkg, 8, INTER_CAN);
    if(get_mcu_num() != 3) return;
    push_packet_to_can(addr_claim_id->allmem, (uint8_t*)&addr_claim_pkg, 8, INTER_CAN);
    push_packet_to_can(addr_claim_id->allmem, (uint8_t*)&addr_claim_pkg, 8, OUT_CAN);
    
}









int its_want_boot_pkg(J1939_ID_t j_id, uint8_t* data){
    J1939_DM14_pkg_t* DM14_pkg = (J1939_DM14_pkg_t*)data;
    uint16_t key = DM14_pkg->key_lsb | (DM14_pkg->key_msb << 8);

    return j_id.PF           == PF_DM14                 &&
           DM14_pkg->command == DM14_COMMAND_BOOT_LOAD  &&
           key               == 0xFFFF;
}
int its_boot_key_pkg(J1939_ID_t j_id, uint8_t* data){
    J1939_DM14_pkg_t* DM14_pkg = (J1939_DM14_pkg_t*)data;
    uint16_t key = DM14_pkg->key_lsb | (DM14_pkg->key_msb << 8);

    return j_id.PF           == PF_DM14                &&
           DM14_pkg->command == DM14_COMMAND_BOOT_LOAD && 
           imageload_state   == IMAGELOAD_GET_WANT     &&
           key               == 0xFEED;
}
int its_our_image_part_pkg(J1939_ID_t j_id){
    return imageload_state == IMAGELOAD_GET_WANT_WITH_KEY &&
           j_id.PF         == PF_DM17;
}
int its_complete_transmission_pkg(J1939_ID_t j_id, uint8_t* data){
    J1939_DM15_pkg_t* DM15_pkg = (J1939_DM15_pkg_t*)data;
    
    return j_id.PF          == PF_DM15                  &&
           DM15_pkg->status == DM15_STATUS_OP_COMPLETED;
}
int its_our_pkg(J1939_ID_t j_id){
    return j_id.PS == BOOTLOADER_CAN1_ADDR;
}
int imageloader_is_active(){
    return imageload_state != IMAGELOAD_NOT_ACTIVE;
           //imageload_state != IMAGELOAD_FINISH;
}







uint32_t get_ptr_from_DM14(uint8_t* data){
    J1939_DM14_pkg_t* DM14_pkg = (J1939_DM14_pkg_t*)data;
    
    uint32_t ptr = (DM14_pkg->pointer_lsb      ) | 
                   (DM14_pkg->pointer_mid << 8 ) | 
                   (DM14_pkg->pointer_msb << 16) | 
                   (0x08) ; // bcse mem region is abstruction for understunding imageload target
    return ptr;
}
uint16_t get_mem_req_from_DM14(uint8_t* data){
    
    J1939_DM14_pkg_t* DM14_pkg = (J1939_DM14_pkg_t*)data; 
    
    return (DM14_pkg->num_req_bytes_lsb) | (DM14_pkg->num_req_bytes_msb << 8);
}







void save_part_of_image(uint8_t* data, uint8_t dlc){
    for(int i = 0; i<dlc; ++i)
        loader_image_buf[loader_image_buf_i++] = data[i];
}
void flash_part_of_image(){
    flash_write(0x08010000, loader_image_buf_i, loader_image_buf);
}







void wait_for_avoid_premature_finish(){
    for(; load_proc_num>0; --load_proc_num) delay_ms(10);
}
void wait_until_imageloader_finish(){
    wait_for_avoid_premature_finish();
    while (imageload_state != IMAGELOAD_FINISH);
}
void wait_until_imageload_activate(){
    while(!imageloader_is_active()); 
}

void __attribute__ ((weak)) _init     (void){}
void __attribute__ ((weak)) _sbrk_r   (void){}
void __attribute__ ((weak)) _close_r  (void){}
void __attribute__ ((weak)) _fstat_r  (void){}
void __attribute__ ((weak)) _isatty_r (void){}
void __attribute__ ((weak)) _lseek_r  (void){}
void __attribute__ ((weak)) _read_r   (void){}

