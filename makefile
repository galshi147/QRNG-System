CC = gcc
# in case we add more driver files in the future
# CFLAGS specifies the compilation flags for the compiler.
# These flags can include options for optimization, debugging, and warning control.
CFLAGS = -I./include -I./FreeRTOS/include -I./drivers -Wall

# SRC specifies the list of source files for the project.
# These files are typically written in a programming language like C or C++ 
# and will be compiled into object files or directly into the final executable.
# Modify this variable to include all the source files required for the build process.
SRC = src/processing.c \
      src/monitor.c \
      src/communication.c \
      src/crc.c \
      src/main.c \
      drivers/hardware_uart.c

OBJ = $(SRC:.c=.o)
TARGET_NATIVE = qrng_system_logic.exe


# --- WOKWI & PYTHON CONFIGURATION---
# Path to the Python visualizer script
VISUALIZER_SCRIPT = scripts/host_reciever.py
# The binary file downloaded from Wokwi Browser
FIRMWARE_BIN = qrng-firmware-esp32.bin

# --- TARGETS ---
# This makefile defines the following phony targets:
# - all   - Typically used to build all default targets in the project.
# - clean - Used to remove build artifacts and clean up the project directory.
# - sim   - Likely used to run simulations (specific to the project context).
# - gui   - Possibly used to launch a graphical user interface for the project.
# - help  - Provides information about the available targets and their usage.
# The `.PHONY` directive ensures that these targets are not treated as files,
# preventing conflicts with files of the same name in the project directory.
.PHONY: all clean sim gui help

# 1. Default: Build the local logic tester
all: $(TARGET_NATIVE)

$(TARGET_NATIVE): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	@echo "[BUILD] Native logic tester built successfully."

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

# 2. Simulation Target: Instructions only
# We now rely on the VS Code Extension for the heavy lifting
sim:
	@echo "[SIM] Checking for firmware binary..."
	@if not exist $(FIRMWARE_BIN) (echo [ERROR] $(FIRMWARE_BIN) missing! Download 'Compiled Firmware' from browser first. & exit 1)
	@echo "[SIM] Please start the simulation inside VS Code:"
	@echo "      1. Open Command Palette (F1)"
	@echo "      2. Select 'Wokwi: Start Simulator'"
	@echo "      (optional: click on diagram.json and press start simulation button)"
	@echo "      3. Verify it is listening on Port 4000"

# 3. GUI Target: Runs the Python Receiver
# Assumes simulation is already running in VS Code
gui:
	@echo "[GUI] Connecting to VS Code Simulation..."
	python $(VISUALIZER_SCRIPT)

# 4. Cleanup
clean:
	rm -f src/*.o drivers/*.o *.o $(TARGET_NATIVE)
	@echo "[CLEAN] Build artifacts removed."

# 5. Help Menu
help:
	@echo "QRNG System Makefile"
	@echo "--------------------"
	@echo "make all   -> Compile local C logic (for unit testing)"
	@echo "make sim   -> Show instructions for starting VS Code Sim"
	@echo "make gui   -> Run Python Visualizer (connects to Port 4000)"
	@echo "make clean -> Remove object files and executables"