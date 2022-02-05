@echo off

call "get_settings.bat"

set cmd=%1

if [%cmd%]==[two] (
    :: sync before running both instances
    ::call "sync_dolphin_instances.bat"

    call "run_dolphin.bat"
    call "run_dolphin.bat" secondary
)
if [%cmd%]==[secondary] (
    call "run_dolphin.bat" secondary
)
if [%cmd%]==[] (
    call "run_dolphin.bat"
)
