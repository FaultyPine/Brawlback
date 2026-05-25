@echo off

:: set target for this build. all means build everything, you can also specify a specific project
set buildTarget=all

:: cmake build - got this command from the console output of running cmake build from the command palette
:: Prepend the PowerPC cross-compiler bin so powerpc-eabi-as is found instead of the host assembler
set PATH=%~dp0BuildSystem\Compiler\bin;%PATH%

set cmakebuild="%CMAKE_EXE_PATH%" --build %CD%/build --config Release --target %buildTarget% -j 14 --
:: move build binaries into SD folder which can then be synced with VSDSync
set pymovefiles=py scripts/moveOutputToSDFolder.py
:: Sync SD folder in this project to the actual sd.raw by running VSDSync.exe
set vsdsync=%CD%/scripts/VSDSync/VSDSync.exe


echo ====================================
echo Building mod files...
echo ====================================

%cmakebuild%
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit /b)
%pymovefiles% 
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit /b)
::echo VSDSync...
::%vsdsync%
::IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit /b)