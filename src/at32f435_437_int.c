#include "at32f435_437_int.h"
#define SLABOUMIE

void NMI_Handler(void){}
void prvGetRegisterFromStack(uint32_t* pulFaultStackAddress){
    volatile uint32_t r0;
    volatile uint32_t r1;
    volatile uint32_t r2;
    volatile uint32_t r3;
    volatile uint32_t r12;
    volatile uint32_t lr;  //link
    volatile uint32_t pc;  //counter
    volatile uint32_t psr; //status
    
    r0  = pulFaultStackAddress[0];
    r1  = pulFaultStackAddress[1];
    r2  = pulFaultStackAddress[2];
    r3  = pulFaultStackAddress[3];
    r12 = pulFaultStackAddress[4];
    lr  = pulFaultStackAddress[5];
    pc  = pulFaultStackAddress[6];
    psr = pulFaultStackAddress[7];
    
    #ifdef SLABOUMIE
    xprintf("HardFault exception!\n");
    xprintf("r0:  %d\n", r0 );
    xprintf("r1:  %d\n", r1 );
    xprintf("r2:  %d\n", r2 ); 
    xprintf("r3:  %d\n", r3 ); 
    xprintf("r12: %d\n", r12); 
    xprintf("lr:  %d\n", lr ); 
    xprintf("pc:  %d\n", pc ); 
    xprintf("psr: %d\n", psr); 
    #endif
    
    while(1){}
}

void HardFault_Handler(void){
    __asm volatile(
        " tst lr, #4                                            \n"
        " ite eq                                                \n"
        " mrseq r0, msp                                         \n"
        " mrsne r0, psp                                         \n"
        " ldr r1, [r0, #24]                                     \n"
        " ldr r2, handler2_address_const                        \n"
        " bx r2                                                 \n"
        " handler2_address_const: .word prvGetRegisterFromStack \n"
    );
}

void MemManage_Handler(void){
    __asm volatile(
        " tst lr, #4                                            \n"
        " ite eq                                                \n"
        " mrseq r0, msp                                         \n"
        " mrsne r0, psp                                         \n"
        " ldr r1, [r0, #24]                                     \n"
        " ldr r2, handler3_address_const                        \n"
        " bx r2                                                 \n"
        " handler3_address_const: .word prvGetRegisterFromStack \n"
    );
}

void BusFault_Handler(void){
__asm volatile(
        " tst lr, #4                                            \n"
        " ite eq                                                \n"
        " mrseq r0, msp                                         \n"
        " mrsne r0, psp                                         \n"
        " ldr r1, [r0, #24]                                     \n"
        " ldr r2, handler5_address_const                        \n"
        " bx r2                                                 \n"
        " handler5_address_const: .word prvGetRegisterFromStack \n"
    );
}

void UsageFault_Handler(void){
__asm volatile(
        " tst lr, #4                                            \n"
        " ite eq                                                \n"
        " mrseq r0, msp                                         \n"
        " mrsne r0, psp                                         \n"
        " ldr r1, [r0, #24]                                     \n"
        " ldr r2, handler4_address_const                        \n"
        " bx r2                                                 \n"
        " handler4_address_const: .word prvGetRegisterFromStack \n"
    );
}

void SVC_Handler(void){}
void DebugMon_Handler(void){}
void PendSV_Handler(void){}
void SysTick_Handler(void){}


