@echo off

for /d %%a in (C:\ti\uniflash_*) do call %%a\dslite.bat -c TM4C123GH6PM.ccxml -e -f -v red_firmware.out
pause
