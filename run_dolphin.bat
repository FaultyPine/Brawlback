@echo off

call "get_settings.bat"

:: launchers
::set launcher="vBrawl.elf"
::set launcher="Project+.elf"
::set launcher="PM.elf"

set dolphin_exe=%DOLPHIN_EXE_PATH%

:: if arg is secondary, run a seperate instance of dolphin
if [%1]==[secondary] (
    set dolphin_exe=%DOLPHIN_SECONDARY_EXE_PATH%
)

set build_dir="%CD%/SDCard"

set run_cmd=start /realtime "" "%dolphin_exe%" --exec "%build_dir%/%LAUNCHER%" -d

echo ===================
echo Starting dolphin...
echo ===================

%run_cmd%