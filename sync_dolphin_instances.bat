@echo off

:: (CHANGEME)
set dolphin_binaries_dir=C:\Users\gclar\Desktop\dolphin\Binary

:: (Might not exist if building on other platform???)
set dolphin_main_dir=%dolphin_binaries_dir%\x64
:: (MAKE SURE TO CREATE THIS)
set dolphin_secondary_dir=%dolphin_binaries_dir%\Secondary

set sd_raw_rel_path=\User\Wii

echo ==================================
echo Synchronizing dolphin instances...
echo ==================================

:: When built, dolphin copies   Dolphin.exe  DSPTool.exe  Updater.exe   into the binaries folder

:: probably only really need to copy Dolphin.exe to sync here
:: DSPTool.exe is a dev tool for dumping opcodes or something like that, and the updater obv isn't being used rn

xcopy /y /q "%dolphin_main_dir%\Dolphin.exe" "%dolphin_secondary_dir%"
::xcopy /y /q "%dolphin_main_dir%\DSPTool.exe" "%dolphin_secondary_dir%"

:: copy sd.raw which contains our code mods
xcopy /y /q "%dolphin_main_dir%\%sd_raw_rel_path%\sd.raw" "%dolphin_secondary_dir%\%sd_raw_rel_path%\sd.raw"

echo ==============================
echo Synchronized dolphin instances
echo ==============================