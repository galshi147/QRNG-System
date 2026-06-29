import serial
import time
from host_receiver import get_connection_url

PORT = get_connection_url()
BAUDRATE = 115200

# Simple script to read and display raw data from the specified serial port
try:
    ser = serial.serial_for_url(url=PORT, baudrate=BAUDRATE, timeout=1)
    print(f"Checking for raw data on {PORT}... (Press Ctrl+C to stop)")
    
    while True:
        if ser.in_waiting > 0:
            data = ser.read(ser.in_waiting)
            print(f"[{time.strftime('%H:%M:%S')}] Received {len(data)} bytes: {data.hex(' ')}")
        else:
            time.sleep(0.1)
except Exception as e:
    print(f"Error: {e}")