REM set variables used in script
set nslaves=2
set gman_ip=127.0.0.1
set gman_port=4005
set pestpp_file=storage5

REM create directories for PEST++ and the gslave to run in
xcopy /e /q /y template master\
FOR /L %%i IN (1,1,%nslaves%) DO (
  xcopy /e /q /y template slave%%i\
)

REM start GMAN
start  /D"%CD%" .\gman64 /ip %gman_ip% /port %gman_port%

REM start GSLAVES
FOR /L %%i IN (1,1,%nslaves%) DO (
 echo %%i
  start /D"%CD%\slave%%i" .\gslave64 /interval 1.0 /console on /name Slave%%i /host %gman_ip%:%gman_port%
)

REM start PEST++
start /D"%CD%\master" .\pest++ %pestpp_file% /G %gman_ip%:%gman_port%
