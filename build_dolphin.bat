@echo off

:: (CHANGEME) path to your vcvars batch script that sets up the dev environment
:: set vcvarsall_path="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
:: %vcvarsall_path%

:: (CHANGEME) path to your dolphin project .sln file.
set project_sln_file="C:\Users\gclar\Desktop\dolphin\Source\dolphin-emu.sln"

:: (CHANGEME) path to your visual studio 2022 (yes, it has to be 2022) devenv.exe
set devenv_path="C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.com"

set build_cmd=%devenv_path% %project_sln_file% /Build Release

echo ====================================
echo Building dolphin...
echo ====================================

%build_cmd%

