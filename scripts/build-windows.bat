@echo off
setlocal

if "%vs140comntools%" neq "" (
	call "%vs140comntools%vsvars32.bat"
) else if "%vs120comntools%" neq "" (
	call "%vs120comntools%vsvars32.bat"
) else (
	echo vs variable not exists.
	exit /b 1
)

if "%qt_dir%" == "" (
	echo 'qt_dir' variable not exists.
	exit /b 2
)
set path=%path%;%qt_dir%

pushd ..
	set project_root=%cd%
	
	::
	echo build peregrine-desktop
	rmdir /s/q build-peregrine-desktop
	mkdir build-peregrine-desktop
	pushd build-peregrine-desktop
		qmake -spec win32-msvc2015 ..\peregrine-desktop\peregrine-desktop.pro
		nmake
	popd
	
	::
	echo copy build outputs
	rmdir /s/q output
	mkdir output
	pushd output
		copy %project_root%\build-peregrine-desktop\release\peregrine-desktop.exe .
		copy "%qt_dir%\Qt5Core.dll" .
		copy "%qt_dir%\Qt5Widgets.dll" .
		copy "%qt_dir%\Qt5Gui.dll" .
	popd

popd

endlocal

exit /b 0