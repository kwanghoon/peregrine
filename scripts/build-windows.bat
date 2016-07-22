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

set config=%1
if "%config%" == "" (
	set config=debug
)
if "%1" neq "release" if "%1" neq "debug" (
	echo "Invalid config specified(%1). it must be debug or release."
	exit /b 3
)
if "%config%" == "debug" (
	set d_suffix=d
)

pushd ..
	set project_root=%cd%
	
	::
	echo build peregrine-desktop
	rmdir /s/q build-peregrine-desktop
	mkdir build-peregrine-desktop
	pushd build-peregrine-desktop
		if "%1" == "debug" (
			qmake -spec win32-msvc2015 "CONFIG+=debug" "CONFIG+=qml_debug" ..\peregrine-desktop\peregrine-desktop.pro
		) else (
			qmake -spec win32-msvc2015 ..\peregrine-desktop\peregrine-desktop.pro
		)
		nmake
	popd
	
	::
	echo copy build outputs
	rmdir /s/q output
	mkdir output
	pushd output
		copy %project_root%\build-peregrine-desktop\%config%\peregrine-desktop.exe .
		copy "%qt_dir%\Qt5Core%d_suffix%.dll" .
		copy "%qt_dir%\Qt5Widgets%d_suffix%.dll" .
		copy "%qt_dir%\Qt5Gui%d_suffix%.dll" .
	popd

popd

endlocal

exit /b 0