@echo off
SETLOCAL
SETLOCAL ENABLEDELAYEDEXPANSION


  
    CMD /C dpinst_64_eng.exe /SE /SW /SA /PATH %~dp0\ccs_base/emulation/drivers/CC3200/ftdi
  

  
    CMD /C dpinst_64_eng.exe /SE /SW /SA /PATH %~dp0\ccs_base/emulation/drivers/stellaris/ftdi_208
  

  
    CMD /C dpinst_64_eng.exe /SE /SW /SA /PATH %~dp0\ccs_base/emulation/drivers/stellaris/icdi
  
