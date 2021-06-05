@echo off
SETLOCAL
SETLOCAL ENABLEDELAYEDEXPANSION


  
    CMD /C dpinst_64_eng.exe /SE /SW /SA /PATH %~dp0\ccs_base/emulation/windows/ftdi_drivers
  

  
    CMD /C dpinst_64_eng.exe /SE /SW /SA /PATH %~dp0\ccs_base/emulation/windows/icdi_drivers
  
