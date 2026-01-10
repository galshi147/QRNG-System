#include "protocol.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"

#define MAX_READ 64
#define MAX_REP 10
#define TIMEOUT 5000 //timeout (ms) for reading from xRandomStreamBufferToMonitor
#define APT_MIN 40 // minimum acceptable number of 1 bits in a 16 bytes (128 bits) string
#define APT_MAX 90 // maximum acceptable number of 1 bits in a 16 bytes (128 bits) string

extern StreamBufferHandle_t xRandomStreamBufferToMonitor;
extern StreamBufferHandle_t xRandomStreamBuffer;

typedef enum {
    SYSTEM_HEALTHY = 0,
    SYSTEM_FAULT,
} MonitorStatus_t;

static MonitorStatus_t system_state = SYSTEM_HEALTHY;

/*
* Function to calculate Hamming Weight (number of 1 bits) in a data array
* Brian Kernighan Algorithm:
* 1. Initialize a counter to zero.
* 2. For each byte in the data array:
*    a. While the byte is greater than zero:
*       i. Perform bitwise AND of the byte with (byte - 1) to clear the least significant set bit.
*       ii. Increment the counter.
* 3. Return the total count of 1 bits.
*/
uint16_t calculate_hamming_weight(uint8_t *data, uint8_t len) {
    uint16_t total_ones = 0;
    
    for (int i = 0; i < len; i++) {
        uint8_t n = data[i];
        // Brian Kernighan Algorithm
        while (n > 0) {
            n &= (n - 1);
            total_ones++;
        }
    }
    return total_ones;
}

void vMonitorTask(void *pvParameters){
    static uint8_t repetitionCount = 1;
    static uint8_t last_byte = 0;
    static uint8_t bytesRead[MAX_READ];
    while (1)
    {        
        size_t bytesReadNum = xStreamBufferReceive(
            xRandomStreamBufferToMonitor,    // which buffer to read from
            bytesRead,                      // where to store the read data
            MAX_READ,                      // max number of bytes to read (up to 64)
            pdMS_TO_TICKS(TIMEOUT)       // how long to wait (Timeout)
        );

        if (bytesReadNum == 0) {
            system_state = SYSTEM_FAULT;
            continue; // in case the detector was broken and no data was read
        }
        // RCT (Repetition Count Test) check
        for (int i = 0; i < bytesReadNum; i++){
            if (bytesRead[i] == last_byte)
            {
                repetitionCount++;
            }
            else
            {
                last_byte = bytesRead[i];
                repetitionCount = 1;
            }
            if (repetitionCount >= MAX_REP)
            {
                system_state = SYSTEM_FAULT;
            }
        }
        // APT (Adaptive Proportion Test) check
        uint16_t total_ones = calculate_hamming_weight(bytesRead, bytesReadNum);
        if (total_ones < APT_MIN || total_ones > APT_MAX){
            system_state = SYSTEM_FAULT;
        }

        if (system_state == SYSTEM_HEALTHY){
            xStreamBufferSend(xRandomStreamBuffer, bytesRead, bytesReadNum, 0);
        }
        else{
            xStreamBufferReset(xRandomStreamBufferToMonitor);
        }
    }

}
