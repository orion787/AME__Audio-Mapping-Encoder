@echo off
set executable=final.exe ..\example.mp3

echo Starting execution time measurement for %executable%

powershell -command "Measure-Command { .\%executable% } | ForEach-Object { $_.TotalSeconds }"

echo Execution completed
pause
