#include <stdint.h>

/* 
* UART_STATUS_REG hardware's status register - tells if TX is ready
* TX_EMPTY_BIT_INDEX bit inside UART_STATUS_REG indicates if TX is ready to accept new data (1 = ready)
* UART_DATA_REG hardware's data register - where to write data to be sent over UART
*/

// addresses from datasheet (this is just a simulation)
#define UART_STATUS_REG  (*(volatile uint32_t *)0x4000C000)
#define UART_DATA_REG    (*(volatile uint32_t *)0x4000C004)
#define TX_EMPTY_BIT_INDEX     (1 << 5) // assume bit 5 indicates TX ready (simulation)

void hardware_uart_send(uint8_t *data, uint16_t len){
    for (int i = 0; i < len; i++){
        while(!(UART_STATUS_REG & TX_EMPTY_BIT_INDEX)){}
        UART_DATA_REG = data[i];
    }
}