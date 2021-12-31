@echo off

:: launch either vBrawl or Project+
set launcher="vBrawl.elf"
::set launcher="Project+.elf"

:: (CHANGEME) path to the built dolphin executable
set dolphin_exe="C:\Users\gclar\Desktop\dolphin\Binary\x64\Dolphin.exe"

:: if arg is secondary, run a seperate instance of dolphin
if [%1]==[secondary] (
    :: (CHANGEME) path to a second dolphin instance for debugging netplay
    set dolphin_exe="C:\Users\gclar\Desktop\dolphin\Binary\Secondary\Dolphin.exe"
)

set build_dir="%CD%/SDCard"

set run_cmd=start /realtime "" "%dolphin_exe%" --exec "%build_dir%/%launcher%" -d

echo ===================
echo Starting dolphin...
echo ===================

%run_cmd%