@echo off
setlocal

if "%vs140comntools%" neq "" (
	call "%vs140comntools%vsvars32.bat"
) else if "%vs120comntools%" neq "" (
	call "%vs120comntools%vsvars32.bat"
) else (
	echo vs variable not exists.
	goto EXIT_FAILED
)

where qmake
if %errorlevel% neq 0 (
	echo couldn't run 'qmake'
	goto EXIT_FAILED
)

set config=%1
if "%config%" == "" (
	set config=debug
)
if "%config%" neq "release" if "%config%" neq "debug" (
	echo "Invalid config specified(%1). it must be debug or release."
	goto EXIT_FAILED
)

pushd ..
	set project_root=%cd%
	
	::
	echo build peregrine
	rmdir /s/q build-peregrine
	timeout 3
	mkdir build-peregrine
	pushd build-peregrine
		if "%config%" == "debug" (
			qmake -spec win32-msvc2015 "CONFIG+=debug" "CONFIG+=qml_debug" ../peregrine.pro
		) else (
			qmake -spec win32-msvc2015 ../peregrine.pro
		)
		nmake
		
		set failed=0
		if not exist peregrine-desktop\%config%\peregrine-desktop.exe (
			set failed=1
		)
	popd
popd

if "%failed%" neq "0" (
	echo compilation failed
	goto EXIT_FAILED
)

echo copy build outputs
call copy-outputs.bat %config%
if "%errorlevel%" neq "0" goto EXIT_FAILED

:EXIT_SUCCEEDED
echo build success
endlocal
exit /b 0

:EXIT_FAILED
echo build failed
endlocal
exit /b 1