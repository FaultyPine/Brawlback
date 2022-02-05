@echo off
setlocal enabledelayedexpansion

clear


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