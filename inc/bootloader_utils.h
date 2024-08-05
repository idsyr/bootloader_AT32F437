#ifndef BOOTLOADER_UTLIS_H
#define BOOTLOADER_UTILS_H 


#include "can.h"
#include "delay.h"
#include "flash.h"
#include "J73.h"
#include "usart.h"
#include "xprintf.h"
#include "gpio.h"
#include "mcu_recog.h"

#include "boot_data_works.h"
#include "jump_to.h"

#include "at32f435_437_clock.h"
#include "at32f435_437_conf.h"
#include "string.h"


#define DEFAULT_MEM_REGION   (uint32_t)0x08010000 
#define BOOTLOADER_CAN1_ADDR 0x83
#define INTER_CAN CAN1
#define OUT_CAN CAN2


typedef enum {
    IMAGELOAD_NOT_ACTIVE,
    IMAGELOAD_GET_WANT,
    IMAGELOAD_MUST_RESPONCE_ON_GET_WANT,
    IMAGELOAD_GET_WANT_WITH_KEY,
    IMAGELOAD_FINISH, 
} IMAGELOAD_STATE;


//can
void print_msg(J1939_ID_t j_id, uint8_t* payload, uint8_t DLC);
void push_packet_to_another_can(uint32_t ext_id, uint8_t* data, uint8_t DLC, can_type* HANDLER_CAN);
void push_packet_to_can(uint32_t ext_id, uint8_t* data, uint8_t DLC, can_type* CANX);

//flash
void save_part_of_image(uint8_t* data, uint8_t dlc);
void flash_part_of_image();

//wait
void wait_for_avoid_premature_finish();
void wait_until_imageloader_finish();
void wait_until_imageload_activate();

//bool
int its_want_boot_pkg(J1939_ID_t j_id, uint8_t* data);
int its_boot_key_pkg(J1939_ID_t j_id, uint8_t* data);
int its_our_image_part_pkg(J1939_ID_t j_id);
int its_will_be_other_mcu_pkg(J1939_ID_t j_id, uint8_t* data);
int its_complete_transmission_pkg(J1939_ID_t j_id, uint8_t* data);
int its_our_pkg(J1939_ID_t j_id);
int imageloader_is_active();

//get
uint32_t get_ptr_from_DM14(uint8_t* data);
uint16_t get_mem_req_from_DM14(uint8_t* data);

//J_net
void J81_formur_this_addr();
void addr_claim();

#endif
