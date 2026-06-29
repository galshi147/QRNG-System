#include <stdint.h>

// Industry Best Practice: Platform Auto-Detection
// ESP_PLATFORM is automatically injected by the ESP-IDF compiler.
// If it is missing, we are compiling locally via GCC for simulation.
#ifndef ESP_PLATFORM
    #define USE_BARE_METAL_POLLING
#endif

#ifdef USE_BARE_METAL_POLLING

// Educational bare-metal polling driver for UART transmission
/* 
* UART_STATUS_REG hardware's status register - tells if TX is ready
* TX_EMPTY_BIT_MASK bit inside UART_STATUS_REG indicates if TX is ready to accept new data (1 = ready)
* UART_DATA_REG hardware's data register - where to write data to be sent over UART
*/

// addresses from datasheet (this is just a simulation)
#define UART_STATUS_REG  (*(volatile uint32_t *)0x4000C000)
#define UART_DATA_REG    (*(volatile uint32_t *)0x4000C004)
#define TX_EMPTY_BIT_MASK     (1 << 5) // assume bit 5 indicates TX ready (simulation)
#define UART_TX_TIMEOUT       10000    // prevent infinite hang if hardware fails

void hardware_uart_init(void) {
    // Bare-metal simulation requires no explicit initialization 
    // as it writes directly to memory-mapped registers.
}

void hardware_uart_send(uint8_t *data, uint16_t len){
    for (int i = 0; i < len; i++){
        uint32_t timeout = UART_TX_TIMEOUT;
        while(!(UART_STATUS_REG & TX_EMPTY_BIT_MASK)){
            if (--timeout == 0) {
                // Hardware stalled: break to prevent RTOS infinite hang
                return;
            }
        }
        UART_DATA_REG = data[i];
    }
}

#else

// --- INDUSTRY STANDARD ESP-IDF RTOS DRIVER ---
#include "driver/uart.h"

void hardware_uart_init(void) {
    // Configure baud rate, parity, and stop bits
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_0, &uart_config);
    
    // Install driver with 256-byte internal TX/RX buffers (turns on interrupts/DMA automatically)
    uart_driver_install(UART_NUM_0, 256, 256, 0, NULL, 0);
}

void hardware_uart_send(uint8_t *data, uint16_t len){
    // uart_write_bytes automatically copies data to a background buffer,
    // initiates hardware DMA transfer, and yields the CPU to other tasks!
    uart_write_bytes(UART_NUM_0, (const char *)data, len);
}

#endif