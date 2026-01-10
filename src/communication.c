#include "protocol.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"


extern StreamBufferHandle_t xRandomStreamBuffer; // buffer from processing task

void vCommunicationTask(void *pvParameters) {
    QRNG_Packet_t packet;
    packet.header_msb = SOF_MSB;
    packet.header_lsb = SOF_LSB;

    const TickType_t xMaxWaitTime = pdMS_TO_TICKS(PACKET_TIMEOUT_MS); // 5 second timer

    while (1) {
        /*
         * try reading up to 64 bytes from the buffer.
         * if no bytes are available within 5 seconds,
         * the function will return with the number of bytes it managed to collect up to that point.
         */
        size_t bytesRead = xStreamBufferReceive(
            xRandomStreamBuffer,    // which buffer to read from
            (void *)packet.payload, // where to store the read data
            MAX_PAYLOAD_SIZE,       // max number of bytes to read (up to 64)
            xMaxWaitTime            // how long to wait (Timeout)
        );

        if (bytesRead > 0) {
            // update the length field in the packet based on what was actually read
            packet.length = (uint8_t)bytesRead;

            // calculate CRC on the data (ensures the computer detects corruption)
            packet.crc = calculate_crc8_fast(packet.payload, packet.length);

            // send the complete packet over UART 
            // 3 bytes of Header+Len, plus the length of the Payload, plus 1 bytes of CRC
            uint16_t totalSize =  PACKET_OVERHEAD_SIZE + packet.length;
            hardware_uart_send((uint8_t *)&packet, totalSize);
        }
        
        // if bytesRead is 0, it means 5 seconds have passed without even a single random byte being generated!
        // here you can send an error message or activate the monitoring task.
        
    }
}