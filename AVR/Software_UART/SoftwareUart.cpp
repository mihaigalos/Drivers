/*
 * SoftwareUart.cpp
 *
 * Created: 4/20/2017 9:29:03 PM
 *  Author: Mihai Galos
 */ 
#include "SoftwareUart.h"

#ifndef F_CPU
    #error Please #define F_CPU
#endif

// Internal computing macros
#define BIT_TIME_NANOSEC 1000000000UL/FIXED_BAUD_RATE 
#define ONE_CLOCK_CYCLE_NANOSEC (1000000000UL/F_CPU)
#define CLOCK_CYCLES_PER_FULL_WAIT_LOOP 3 // using 3cc for each iteration
#define PRESCALE_WAIT_ONE_BIT ((BIT_TIME_NANOSEC/ONE_CLOCK_CYCLE_NANOSEC)/(CLOCK_CYCLES_PER_FULL_WAIT_LOOP)) 
#define PRESCALE_WAIT_HALF_BIT 

#define PRESCALE_WAIT_ONE_BIT_RX PRESCALE_WAIT_ONE_BIT
#define PRESCALE_WAIT_HALF_BIT_RX (static_cast<uint8_t>(PRESCALE_WAIT_ONE_BIT_RX)) / 2

#define INSTRUCTION_OFFSET 7 // clock cycles needed after setting a pin hi/lo before starting bitDelaySend
#define PRESCALE_WAIT_ONE_BIT_TX (PRESCALE_WAIT_ONE_BIT - INSTRUCTION_OFFSET)

#if PRESCALE_WAIT_ONE_BIT_RX == 0
    #ifdef F_CPU
        #error PRESCALE_WAIT_ONE_BIT_RX is 0. Try decreasing the Baudrate.
    #endif
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#pragma message "assuming clock: " STR(F_CPU) " Mhz."

void uart_init(){
    UART_DDR |= (1<<TX_PIN);
    UART_DDR &=~(1<<RX_PIN);
    UART_OUT_PORT_MAPPING |= (1<<TX_PIN); // Tx line high when idle
}


uint8_t uart_read(){ 
    uint8_t temporary1 =0;
    uint8_t readValue;

    __asm__ volatile (
        "wait: \n\t"
            "in %2, %6 \n\t"      
            "andi %2, %1 \n\t"       
            "brne wait \n\t"        
            "rcall halfDelay \n\t"  
            "in %2, %6 \n\t"
            "andi %2, %1 \n\t"
            "brne wait \n\t"
            "rcall bitDelayReceive \n\t"
        
        "read8bits: \n\t"           // read in 8 bits
            "in %2, %6 \n\t" 
            "andi %2, %3 \n\t"
            "brne skipBitSet \n\t"
                "or %0, %3 \n\t"
                
        "skipBitSet: \n\t"
            "lsl %3 \n\t"
            "breq eof_read8bits \n\t"
            "rjmp read8bits \n\t"
        
       // delay routines
       "bitDelayReceive: \n\t"
            "mov %2, %5 \n\t"
            //"ldi %2, 255 \n\t"
            "rjmp loop_3cc \n\t"
       
       "halfDelay: \n\t"             // 3cc per loop iteration
           "ldi %2, %4 \n\t"        // 1cc
           "loop_3cc: \n\t"
           "dec %2 \n\t"            // 1cc
           "brne loop_3cc \n\t"      // 2cc (true), 1cc (false)
           "ret \n\t"
       
        // done
        "eof_read8bits: \n\t" 
        : "=&r" (readValue)
        : "M" (RX_PIN), "r" (temporary1), "M" (1), "M" (PRESCALE_WAIT_HALF_BIT_RX) , "r" (PRESCALE_WAIT_ONE_BIT_RX), "M" (_SFR_IO_ADDR (UART_IN_PORT_MAPPING))
        
    );
    return readValue;
}

void uart_write(uint8_t value){ // TODO : pin should only have one bit set
    uint8_t temporary =0, currentBitIndex = 8; // 8 bits
    __asm__ volatile(
    
    "cbi %4, %0 \n\t"           // falling edge : start condition
    "rcall bitDelaySend \n\t"
    "write8bits: \n\t"           // read in 8 bits
        
        "mov %5, %1 \n\t"
        "andi %1, %2 \n\t"           
        "breq setPinLow \n\t"       // and == 0, meaning LSB was 0, set bit low
        "sbi %4, %0 \n\t"           // and == 1, meaning LSB was 1, set bit high
        "rjmp pinSetFinished \n\t"  // .. and jump to pinSetFinished*/
        "setPinLow: \n\t"
            "cbi %4, %0 \n\t"
            "nop \n\t"              // additional no operation to balance the clock cycle count when taking the previous branches
        "pinSetFinished:"
            "mov %1, %5 \n\t"
            "lsl %1 \n\t"
            "rcall bitDelaySend \n\t"
            "dec %3 \n\t"
            "brne write8bits \n\t"
            "rjmp eof_write8bits \n\t"
        
     "bitDelaySend: \n\t"
        "mov %5, %6 \n\t"   
        "repeat_3cc: \n\t"
            "dec %5 \n\t"            // 1cc
            "brne repeat_3cc \n\t"      // 2cc (true), 1cc (false)
            "ret \n\t"
    // done
    "eof_write8bits: \n\t"
        "sbi %4, %0 \n\t"               // stop bit
        "rcall bitDelaySend \n\t"           
    : 
    : "M" (TX_PIN), "r" (value), "M" (0x80), "r"(currentBitIndex), "M" (_SFR_IO_ADDR (UART_OUT_PORT_MAPPING)), "r" (temporary), "r" (PRESCALE_WAIT_ONE_BIT_TX)
    
    );
     
}

