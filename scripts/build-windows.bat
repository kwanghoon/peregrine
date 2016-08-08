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
	mkdir build-peregrine
	pushd build-peregrine
		if "%config%" == "debug" (
			qmake -spec win32-msvc2015 "CONFIG+=debug" "CONFIG+=qml_debug" ../peregrine.pro
		) else (
			qmake -spec win32-msvc2015 ../peregrine.pro
		)
		nmake
	popd
popd

echo copy build outputs
call copy-outputs.bat %config%

:EXIT_SUCCEEDED
endlocal
exit /b 0

:EXIT_FAILED
endlocal
exit /b 1