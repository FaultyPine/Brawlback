@echo off

call "build.bat"

echo ==========================
echo Packaging dolphin build...
echo ==========================
py scripts/packageBuild.py %DOLPHIN_EXE_PATH%