@echo off

setlocal

where mongod
if "%errorlevel%" neq "0" (
	echo "not found 'mongod' command"
	goto failed
)

set dbdir=..\db
if not exist  %dbdir% (
	mkdir %dbdir%
)

call mongod --dbpath=%dbdir% --port=27017

exit /b 0

:failed
endlocal
exit /b 1