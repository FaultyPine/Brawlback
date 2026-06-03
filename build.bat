@echo off
setlocal enabledelayedexpansion

cls && color

pushd "%~dp0"

:: Kill any running Dolphin instances so build artifacts aren't locked
tasklist /FI "IMAGENAME eq Dolphin.exe" 2>nul | find /I "Dolphin.exe" >nul
if not errorlevel 1 (
    echo Killing running Dolphin.exe processes...
    taskkill /F /IM Dolphin.exe >nul 2>&1
)

call "get_settings.bat"

if [%1]==[] (
    call "build_dolphin.bat" && call "build_mod.bat" && call "sync_dolphin_instances.bat"
    GOTO:eof
)

:: iterate through cmd line arguments
for %%x in (%*) do (

    if [%%x]==[sync] (
        call "sync_dolphin_instances.bat"
    )
    if [%%x]==[dolphin] ( 
        call "build_dolphin.bat"
    ) 
    if [%%x]==[mod] ( 
        call "build_mod.bat"
    )

)

:: just sync after building to ensure instances are always synced
call "sync_dolphin_instances.bat"
popd