#include "protocol.h"
#include "qrng_tasks.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"

StreamBufferHandle_t xRandomStreamBufferToMonitor;
StreamBufferHandle_t xRandomStreamBuffer;

uint8_t rawBuffer[64]; // Buffer for the DMA filled by the ISR with raw ADC data

int main(void) {
    // 1. hardware reset - imaginary functions (ADC, DMA, UART)
    // hardware_init();

    // 2. initiate Stream Buffers
    // Buffer linking processing to monitoring (size 128 bytes, wake threshold 16)
    xRandomStreamBufferToMonitor = xStreamBufferCreate(128, 16);
    
    // Buffer linking monitoring to communication (size 128 bytes, wake threshold 16)
    xRandomStreamBuffer = xStreamBufferCreate(128, 16);

    // 3. create tasks (higher priority for processing and monitoring)
    xTaskCreate(vProcessingTask, "PROC", 256, NULL, 3, NULL);
    xTaskCreate(vMonitorTask,    "MON",  256, NULL, 3, NULL);
    xTaskCreate(vCommunicationTask, "COMM", 256, NULL, 2, NULL);

    // 4. start the Scheduler
    vTaskStartScheduler();

    while(1); // Should never reach here
    return 0;
}