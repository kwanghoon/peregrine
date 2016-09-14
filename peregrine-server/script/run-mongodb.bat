@echo off

setlocal

where mongod
if "%errorlevel%" neq "0" (
	echo "not found 'mongod' command"
	goto failed
)

if not exist ..\db (
	mkdir ..\db
)

call mongod --dbpath=..\db --port=27017

exit /b 0

:failed
endlocal
exit /b 1