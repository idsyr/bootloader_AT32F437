#ifndef MAIN_H
#define MAIN_H

#include "stdint.h"

#include "can.h"
#include "delay.h"
#include "flash.h"
#include "J73.h"
#include "usart.h"
#include "xprintf.h"
#include "gpio.h"

#include "boot_data_works.h"
#include "jump_to.h"

#include "at32f435_437_clock.h"
#include "at32f435_437_conf.h"
#include "string.h"

#include "bootloader_utils.h"


void loader_handler(J1939_ID_t j_id, uint8_t* data, uint8_t DLC, can_type* THIS_CAN);
void responce_on_want_load(uint16_t seed, uint16_t mem_req_amount, can_type* CANX, uint8_t sender);
void init_imageload_resources();


#endif
