::
:: This script enables to define the required environment variables inside the script.
:: It is useful when you don't want to define global variables.
::
setlocal
call set-build-vars.bat
call build-windows.bat
endlocal