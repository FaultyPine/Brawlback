@echo off

call "get_settings.bat"


:: (Might not exist if building on other platform???)
set dolphin_main_dir=%DOLPHIN_BINARIES_DIR%\x64
:: (MAKE SURE TO CREATE THIS)
set dolphin_secondary_dir=%DOLPHIN_BINARIES_DIR%\Secondary

set sd_raw_rel_path=\User\Wii
set map_folder=\User\Maps

echo ==================================
echo Synchronizing dolphin instances...
echo ==================================

:: When built, dolphin copies   Dolphin.exe  DSPTool.exe  Updater.exe   into the binaries folder

:: probably only really need to copy Dolphin.exe to sync here
:: DSPTool.exe is a dev tool for dumping opcodes or something like that, and the updater obv isn't being used rn

xcopy /y /q "%dolphin_main_dir%\Dolphin.exe" "%dolphin_secondary_dir%\Dolphin.exe"*
::xcopy /y /q "%dolphin_main_dir%\DSPTool.exe" "%dolphin_secondary_dir%"

:: copy sd.raw which contains our code mods
xcopy /y /q "%dolphin_main_dir%\%sd_raw_rel_path%\sd.raw" "%dolphin_secondary_dir%\%sd_raw_rel_path%\sd.raw"*

:: copy Symbols.map file to dolphin map folders
xcopy /y /q "%CD%\build\Disassembly\Symbols.map" "%dolphin_main_dir%\%map_folder%\RSBE01.map"
xcopy /y /q "%CD%\build\Disassembly\Symbols.map" "%dolphin_secondary_dir%\%map_folder%\RSBE01.map"

echo ==============================
echo Synchronized dolphin instances
echo ==============================