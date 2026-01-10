#include "protocol.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"

extern StreamBufferHandle_t xRandomStreamBufferToMonitor;
extern uint8_t rawBuffer[RAW_BUFFER_SIZE];

/*
* Processing Task
* This task processes raw data from the rawBuffer filled by the ISR.
* It extracts random bits, constructs bytes, and sends them to the random stream buffer.
* Algorithm:
* 1. Wait for notification from ISR indicating new data is available.
* 2. For each 32-bit (4 byte) word in the rawBuffer:
*    a. For each pair of bits in the word apply von Neumann extractor:
*       i. If the pair is '01', append a '0' bit to the current byte.
*       ii. If the pair is '10', append a '1' bit to the current byte.
*       iii. If the pair is '00' or '11', discard it (no bit is added).
*       iv. Once 8 bits are collected, form a byte and store it in the outBlock buffer.
*       v. If the outBlock buffer is full (16 bytes), send it to the xRandomStreamBuffer and reset the index.
* 3. Repeat the process indefinitely.
*/
void vProcessingTask(void *pvParameters){
    static uint8_t currentByte; // Current byte being constructed from detected bits
    static uint8_t bitsCounter;  // Number of bits collected for the current byte
    static uint8_t outBlock[16]; // Local Buffer to save random bytes and send them to the random buffer as a group
    static uint8_t outBlockIndex; // Index in the local output buffer

    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Wait until notified by the ISR
        for (int i = 0; i < RAW_BUFFER_SIZE; i++){
            for (uint8_t j = 0; j < 4; j++){
                uint8_t bitPair = (rawBuffer[i] >> (2*j)) & 3;
                switch (bitPair)
                {
                case 1:
                    currentByte = (currentByte << 1) | 0;
                    bitsCounter++;
                    break;
                case 2:
                    currentByte = (currentByte << 1) | 1;
                    bitsCounter++;
                    break;
                default:
                    break;
                }
                if (bitsCounter == 8){
                    outBlock[outBlockIndex] = currentByte;
                    outBlockIndex++;
                    currentByte = 0;
                    bitsCounter = 0;
                }
                if (outBlockIndex == 16){
                    xStreamBufferSend(xRandomStreamBufferToMonitor, outBlock, 16, 0);
                    outBlockIndex = 0;
                }
            }
        }
    }
    
}