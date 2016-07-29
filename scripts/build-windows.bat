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
	echo build peregrine
	rmdir /s/q build-peregrine
	mkdir build-peregrine
	pushd build-peregrine
		if "%1" == "debug" (
			qmake -spec win32-msvc2015 "CONFIG+=debug" "CONFIG+=qml_debug" ../peregrine.pro
		) else (
			qmake -spec win32-msvc2015 ../peregrine.pro
		)
		nmake
	popd
	
	::
	echo copy build outputs
	rmdir /s/q output
	mkdir output
	pushd output
		copy %project_root%\build-peregrine\peregrine-desktop\%config%\peregrine-desktop.exe .
		set qtmodules=(Core Widgets Gui QuickWidgets Xml)
		for %%m in %qtmodules% do (
			copy "%qt_dir%\Qt5%%m%d_suffix%.dll" .
		)
		
		robocopy %project_root%\plugins .\plugins *.xml *.png /S
		
		cd ..\build-peregrine\plugins
		for /d %%a in (*) do (
			copy %%a\%config%\%%a.dll %project_root%\output\plugins\%%a\
		)
	popd

popd

endlocal

exit /b 0