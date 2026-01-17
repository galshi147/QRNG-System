param (
    [string]$Target = "gui"
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
    Write-Host "  sim    -> Instructions for Wokwi Simulation (VS Code wokwi extension Required)"
    Write-Host "  gui    -> Run Python Visualizer (Default)" -ForegroundColor Yellow
    Write-Host "  clean  -> Remove temporary Python cache files"
    Write-Host "  help   -> Show this menu"
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
	Write-Host "      3. Verify it is listening on Port 4000"
}

function Start-Gui {
    Write-Host "[GUI] Connecting to VS Code Simulation on port 4000..." -ForegroundColor Cyan
    
    # Friendly check before running
    Write-Host "(*) Info: This script assumes the Wokwi Simulator is ALREADY running." -ForegroundColor Gray
    Write-Host "(*) Action: If connection fails, press F1 -> 'Wokwi: Start Simulator' in VS Code." -ForegroundColor Yellow
    
    python $VisualizerScript
}

function Start-Clean {
    Write-Host "[CLEAN] Removing python cache artifacts..." -ForegroundColor Yellow
    Get-ChildItem -Recurse -Include *.pyc, __pycache__ | Remove-Item -Force -Recurse -ErrorAction SilentlyContinue
    Write-Host "Done."
}

# --- Main Switch ---

switch ($Target.ToLower()) {
    "sim"   { Start-Sim }
    "gui"   { Start-Gui }
    "clean" { Start-Clean }
    "help"  { Show-Help }
    default { Start-Gui }
}