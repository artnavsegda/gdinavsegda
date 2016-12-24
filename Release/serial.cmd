@echo off
:start
set /p serialport="enter serial number: "
if %serialport%==q (
	exit
)
set serialrecordfile=serialrecord%random%.txt
serialrecord \\.\COM%serialport% %serialrecordfile%
if errorlevel 1 (
	goto :start
)
serialmon %serialrecordfile%
goto :start
