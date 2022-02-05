@echo off

:: (CHANGEME) path to your vcvars batch script that sets up the dev environment
:: set vcvarsall_path="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
:: %vcvarsall_path%

set build_cmd="%DEVENV_PATH%" "%PROJECT_SLN_FILE%" /Build Release

echo ====================================
echo Building dolphin...
echo ====================================

%build_cmd%

