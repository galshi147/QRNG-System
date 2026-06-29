param (
    [string]$Target = "all"
)

# --- Configuration ---
$VisualizerScript = "scripts/host_receiver.py"
$FirmwareBin = "qrng-firmware-esp32.bin"

# --- Functions ---
function Show-Help {
    Write-Host "QRNG System Management Script" -ForegroundColor Cyan
    Write-Host "-----------------------------" -ForegroundColor Cyan
    Write-Host "Usage: .\manage.ps1 [target]"
    Write-Host ""
    Write-Host "Targets:"
    Write-Host "  all/build -> Build native C logic (requires gcc/MinGW) (Default)" -ForegroundColor Yellow
    Write-Host "  sim       -> Instructions for Wokwi Simulation (VS Code wokwi extension Required)"
    Write-Host "  gui       -> Run Python Visualizer"
    Write-Host "  clean     -> Remove build artifacts and temporary Python cache files"
    Write-Host "  help      -> Show this menu"
}

function Start-Sim {
    Write-Host "[SIM] Checking for firmware binary..."
    if (-Not (Test-Path $FirmwareBin)) {
        Write-Host "[ERROR] $FirmwareBin missing! Download 'Compiled Firmware' from browser first."
        exit 1
    }
    Write-Host "[SIM] Please start the simulation inside VS Code:"
	Write-Host "      1. Open Command Palette (F1)"
	Write-Host "      2. Select 'Wokwi: Start Simulator'"
	Write-Host "      (optional: click on diagram.json and press start simulation button)"
	Write-Host "      3. Verify it is listening on Port 4000 (e.g., run 'netstat -ano | findstr `":4000`"')"
}

function Start-Gui {
    Write-Host "[GUI] Connecting to VS Code Simulation on port 4000..." -ForegroundColor Cyan
    
    # Friendly check before running
    Write-Host "(*) Info: This script assumes the Wokwi Simulator is ALREADY running." -ForegroundColor Gray
    Write-Host "(*) Action: If connection fails, press F1 -> 'Wokwi: Start Simulator' in VS Code." -ForegroundColor Yellow
    
    python $VisualizerScript
}

function Start-Build {
    Write-Host "[BUILD] Compiling native C logic..." -ForegroundColor Yellow
    
    $Compiler = "gcc"
    $CFlags = "-I./include", "-I./FreeRTOS/include", "-I./drivers", "-Wall"
    $Sources = @(
        "src/processing.c",
        "src/monitor.c",
        "src/communication.c",
        "src/crc.c",
        "src/main.c",
        "drivers/hardware_uart.c"
    )
    $TargetExe = "qrng_system_logic.exe"

    # Check if gcc is available
    if (-not (Get-Command $Compiler -ErrorAction SilentlyContinue)) {
        Write-Host "[ERROR] 'gcc' is not installed or not in your PATH. Please install MinGW/MSYS2." -ForegroundColor Red
        return
    }

    $CommandArgs = $Sources + "-o", $TargetExe + $CFlags
    $ArgsString = $CommandArgs -join " "
    Write-Host "> $Compiler $ArgsString" -ForegroundColor DarkGray
    
    $Process = Start-Process -FilePath $Compiler -ArgumentList $CommandArgs -NoNewWindow -Wait -PassThru
    
    if ($Process.ExitCode -eq 0) {
        Write-Host "[BUILD] Native logic tester built successfully." -ForegroundColor Green
    } else {
        Write-Host "[ERROR] Build failed with exit code $($Process.ExitCode)." -ForegroundColor Red
    }
}

function Start-Clean {
    Write-Host "[CLEAN] Removing build artifacts and python cache..." -ForegroundColor Yellow
    Get-ChildItem -Recurse -Include *.pyc, __pycache__ | Remove-Item -Force -Recurse -ErrorAction SilentlyContinue
    Remove-Item -Force -Path src\*.o, drivers\*.o, *.o, qrng_system_logic.exe -ErrorAction SilentlyContinue
    Write-Host "Done."
}

# --- Main Switch ---

switch ($Target.ToLower()) {
    "all"   { Start-Build }
    "build" { Start-Build }
    "sim"   { Start-Sim }
    "gui"   { Start-Gui }
    "clean" { Start-Clean }
    "help"  { Show-Help }
    default { Start-Build }
}