#include "main.h"

extern IMAGELOAD_STATE imageload_state;
extern uint16_t mem_req_amount;
extern uint32_t ptr; 



void loader_handler(J1939_ID_t j_id, uint8_t* data, uint8_t DLC, can_type* THIS_CAN){

    xprintf("in : "); print_msg(j_id, data, DLC);
    if(!its_our_pkg(j_id)) return;
    if(its_want_boot_pkg(j_id, data)){
        imageload_state = IMAGELOAD_GET_WANT;
        responce_on_want_load(0xDEAF, 2048, THIS_CAN, j_id.SA);
    }
    if(its_boot_key_pkg(j_id, data)){
        imageload_state = IMAGELOAD_GET_WANT_WITH_KEY;
        mem_req_amount  = get_mem_req_from_DM14(data);
        ptr             = get_ptr_from_DM14(data);
        responce_on_want_load(0xFFFF, mem_req_amount, THIS_CAN, j_id.SA);
    }
    if(its_complete_transmission_pkg(j_id, data)){
        imageload_state = IMAGELOAD_FINISH;
        flash_part_of_image();
    }
    if(its_our_image_part_pkg(j_id)){
        save_part_of_image(data, DLC);
    }
}

void CAN1_RX0_IRQHandler(void){
    can_rx_message_type rx_message_struct;

    can_type*  THIS_CAN = CAN1;
    J1939_ID_t j_id_rx;
  
    if(can_interrupt_flag_get(THIS_CAN, CAN_RF0MN_FLAG) != RESET){
        can_message_receive(THIS_CAN, CAN_RX_FIFO0, &rx_message_struct);
        
        j_id_rx.allmem     = rx_message_struct.extended_id;
        uint8_t* data_     = rx_message_struct.data;
        uint8_t  DLC_      = rx_message_struct.dlc;

        loader_handler(j_id_rx, data_, DLC_, THIS_CAN);
    }
}

void CAN1_SE_IRQHandler(void){
    volatile uint32_t err_index = 0;
    if(can_interrupt_flag_get(CAN1, CAN_ETR_FLAG) != RESET){
        xprintf("can error occur\n");
        err_index = CAN1->ests & 0x70;
        can_flag_clear(CAN1, CAN_ETR_FLAG);
        if(err_index == 0x00000010){
            //data error occur
        }
    }
}

void CAN2_RX0_IRQHandler(void){
    can_rx_message_type rx_message_struct;

    can_type* THIS_CAN = CAN2;
    J1939_ID_t j_id_rx;

    if(can_interrupt_flag_get(THIS_CAN, CAN_RF0MN_FLAG) != RESET){
        can_message_receive(THIS_CAN, CAN_RX_FIFO0, &rx_message_struct);
        
        j_id_rx.allmem     = rx_message_struct.extended_id;
        uint8_t* data_     = rx_message_struct.data;
        uint8_t  DLC_      = rx_message_struct.dlc;

        loader_handler(j_id_rx, data_, DLC_, THIS_CAN);
    }
}

void responce_on_want_load(uint16_t seed, uint16_t mem_req_amount, can_type* CANX, uint8_t sender){
    
    J1939_ID_t j_id;
    formur_dm15_id(&j_id, sender);

    J1939_DM15_pkg_t dm15_pkg;
    formur_dm15_pkg(&dm15_pkg, mem_req_amount, seed, DM15_STATUS_PROCEED);
    
    push_packet_to_can(j_id.allmem, (uint8_t*)&dm15_pkg.allmem, 8, CANX);
}

void init_imageload_resources(){
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

    read_boot_data_struct();

    //if(!imageloader_called_from_app()){
        system_clock_config();
        enable_clock_all_gpio();
    //} 

    can_init(CAN1_INDEX);
    
    usart1_config(); xdev_out(usart1_putc); xdev_in(usart1_getc);

    delay_init();
}
//SCB->VTOR
void main(){
    init_imageload_resources();

    xprintf("booloader_start\n");
    xprintf("mcu_num: %d\n", get_mcu_num());
    
    xprintf("interrupt table on start: %02X\n", SCB->VTOR);
    set_interrupt_table_addr(FLASH_BOOTLOADER_START_ADDRESS);
    xprintf("interrupt table adter set: %02X\n", SCB->VTOR);


    if(!app_image_in_memory()){
        xprintf("no_app_image_in_memory\n");
        addr_claim();
        wait_until_imageload_activate();
        wait_until_imageloader_finish();
    }

    if(imageloader_called_from_app()){
        clean_flag_call_from_app();
        clean_flag_image_in_memory();
        write_boot_data_struct_in_flash();

        xprintf("imageloader_called_from_app\n");
        //delay_ms(10);
        imageload_state = IMAGELOAD_GET_WANT;
        responce_on_want_load(0xFADE, 
                              2048,
                              get_canx_from_boot_data(), 
                              get_dm14_id_from_boot_data().SA);
        wait_until_imageloader_finish();
    }
    write_boot_data_struct_in_flash();
    xprintf("jump_to_app\n");
    jump_to_app();
}













