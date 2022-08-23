echo off
set LOCALHOST=%COMPUTERNAME%
set KILL_CMD="C:\PROGRA~1\ANSYSI~1\2022R2\v222\fluent/ntbin/win64/winkill.exe"

"C:\PROGRA~1\ANSYSI~1\2022R2\v222\fluent\ntbin\win64\tell.exe" ZHHL-PC 31060 CLEANUP_EXITING
if /i "%LOCALHOST%"=="ZHHL-PC" (%KILL_CMD% 16944) 
if /i "%LOCALHOST%"=="ZHHL-PC" (%KILL_CMD% 5900) 
if /i "%LOCALHOST%"=="ZHHL-PC" (%KILL_CMD% 18016)
del "F:\ZHHL\TE_Doctor\CASES\case220626\python_fluent\python\DllSocket\fluentUDF\cleanup-fluent-ZHHL-PC-5900.bat"
