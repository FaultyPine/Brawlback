@echo off


for /f "tokens=1,2 delims==" %%a in (build_settings.ini) do (
    :: name of .elf launcher. Launcher is called from BUILD_DIR/LAUNCHER
    if %%a==LAUNCHER set LAUNCHER=%%b
    :: .sln file from the dolphin repo
    if %%a==PROJECT_SLN_FILE set PROJECT_SLN_FILE=%%b
    :: path to devenv.com file that comes with visual studio (must be 2022)
    if %%a==DEVENV_PATH set DEVENV_PATH=%%b
    :: path to cmake.EXE
    if %%a==CMAKE_EXE_PATH set CMAKE_EXE_PATH=%%b
    :: path to dolphin repo Binary folder
    if %%a==DOLPHIN_BINARIES_DIR set DOLPHIN_BINARIES_DIR=%%b
    :: path to primary Dolphin.exe
    if %%a==DOLPHIN_EXE_PATH set DOLPHIN_EXE_PATH=%%b
    :: path to secondary Dolphin.exe
    if %%a==DOLPHIN_SECONDARY_EXE_PATH set DOLPHIN_SECONDARY_EXE_PATH=%%b
)
