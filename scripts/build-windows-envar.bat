::
:: This script enables to define the required environment variables inside the script.
:: It is useful when you don't want to define global variables.
::
setlocal
	set qt_dir=G:\QT\Qt5.7.0\5.7\msvc2015\bin
	build-windows.bat %1 %2 %3 %4
endlocal