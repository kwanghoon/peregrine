@echo off
setlocal

set config=%1
if "%config%" == "" (
	set config=debug
)
if "%config%" neq "release" if "%config%" neq "debug" (
	echo "Invalid config specified(%1). it must be debug or release."
	goto EXIT_FAILED
)

if not exist ..\build-peregrine\peregrine-test\%config%\peregrine-test.exe (
	goto EXIT_FAILED
)

echo run test
..\build-peregrine\peregrine-test\%config%\peregrine-test.exe

:EXIT_SUCCEEDED
echo test success
endlocal
exit /b 0

:EXIT_FAILED
echo test failed
endlocal
exit /b 1