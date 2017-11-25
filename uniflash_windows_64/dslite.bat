@echo off
setlocal
setlocal ENABLEDELAYEDEXPANSION

REM Path to this batch script
set UNIFLASH_PATH=%~dp0

REM Path to DebugServer folder
set DEBUGSERVER_ROOT=%~dp0ccs_base\DebugServer\

set MODE=flash
set EXECUTABLE="!DEBUGSERVER_ROOT!bin\DSLite"

set GENERATED_COMMAND=-c user_files/configs/tm4c123gh6pm.ccxml -l user_files/settings/generated.ufsettings -e -f -v user_files/images/red_firmware.out 

REM list available modes
if "%1"=="--listMode" (
	echo.
	
	echo Usage: dslite --mode ^<mode^> arg
	echo.
	
	echo Available Modes for UniFlash CLI:
	echo   * flash [default] - on-chip flash programming
	echo   * memory          - export memory to a file
	echo   * load            - simple loader [use default options]
	echo   * serial          - serial flash programming
if exist !DEBUGSERVER_ROOT!drivers\MSP430Flasher.exe (
	echo   * mspflasher      - support MSPFlasher command line parameters [deprecated]
)
	
	exit /b 0
)

REM no parameters given, use the default generated command
if "%1" EQU "" (
	echo Executing default command:
	echo ^> dslite --mode !MODE! !GENERATED_COMMAND!
	echo.
	
	CMD /S /C "%EXECUTABLE% !MODE! !GENERATED_COMMAND!"
	pause
	exit /b !errorlevel!
)

REM user options parsing
set USEROPTIONS=%*

REM user options without the --mode
set "_args=%*"
set "_args=!_args:*%1 =!"
set "_args=!_args:*%2 =!"

if "%3"=="" (
	set USEROPTIONS2=
) else (
	set USEROPTIONS2=!_args!
)

REM custom mode from users
if "%1" EQU "--mode" (
	set MODE=%2
	set USEROPTIONS=!USEROPTIONS2!
)

REM default user options if none given
if "!USEROPTIONS!" EQU "" (
	set USEROPTIONS=-h
)

REM mspflasher support
if "%MODE%" EQU "mspflasher" (
	set EXECUTABLE=!DEBUGSERVER_ROOT!drivers\MSP430Flasher.exe
	set MODE=
)

REM execute with given user parameters
echo Executing the following command:
if "!MODE!" EQU "" (
	echo ^> !EXECUTABLE! !USEROPTIONS!
) else (
	echo ^> !EXECUTABLE! !MODE! !USEROPTIONS!
)
echo.

CMD /S /C "%EXECUTABLE% !MODE! !USEROPTIONS!"
exit /b !errorlevel!