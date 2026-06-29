#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#define SOF_MSB 0xAA  // Start of Frame
#define SOF_LSB 0x55
#define MAX_PAYLOAD_SIZE 64
#define PACKET_TIMEOUT_MS 5000
#define PACKET_OVERHEAD_SIZE 4 // Header(2) + Length(1) + CRC(1)
#define RAW_BUFFER_SIZE 64 // Size of the raw buffer filled by the ISR
#define CRC8_POLYNOMIAL 0x07 // x^8 + x^2 + x + 1

// packet structure
typedef struct __attribute__((packed)) {
    uint8_t header_msb;     // 0xAA
    uint8_t header_lsb;     // 0x55
    uint8_t length;         // length of payload
    uint8_t payload[MAX_PAYLOAD_SIZE + 1]; // +1 room for dynamically placed CRC
} QRNG_Packet_t;


uint8_t calculate_crc8(uint8_t *data, uint8_t len);
uint8_t calculate_crc8_fast(uint8_t *data, uint8_t len);
uint16_t calculate_hamming_weight(uint8_t *data, uint8_t len);

// Function to send data over UART (to be implemented in hardware_uart.c)
void hardware_uart_send(uint8_t *data, uint16_t len);

#endif