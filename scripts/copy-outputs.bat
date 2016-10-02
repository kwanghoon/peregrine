::
:: copy build results to the output directory.
::
:: arguments
::   %1 - compile optimization mode. debug or release.
::   %2 - build result dir. it's optional.
::
@echo off
setlocal

set optim_mode=%1
if "%optim_mode%" == "" set optim_mode=debug
set project_root_dir=%~dp0..
if "%2" == "" (
	if not exist "%~dp0..\build-peregrine" (
		echo "Not build results found! (root/build-peregrine)"
		goto EXIT_FAILED
	)
	set build_result_dir=%project_root_dir%\build-peregrine
) else (
	set build_result_dir=%2
)
set output_dir=%project_root_dir%\output

rmdir /s/q %output_dir%
:: get some delay to work around rmdir's latency problem.
timeout 3
mkdir %output_dir%

:: app binaries & QT dlls
echo copy app binaries..
copy %build_result_dir%\peregrine-desktop\%optim_mode%\peregrine-desktop.exe %output_dir%

:: qt dlls
if "%vs140comntools%" neq "" (
	call "%vs140comntools%vsvars32.bat"
) else if "%vs120comntools%" neq "" (
	call "%vs120comntools%vsvars32.bat"
) else (
	echo vs variable not exists.
	goto EXIT_FAILED
)

where windeployqt
if %errorlevel% neq 0 (
	echo couldn't run 'windeployqt'
	goto EXIT_FAILED
)
pushd %output_dir%
	windeployqt .
popd

:: qt qml dlls
SET TEMPFILE=%TEMP%\TEMP-%DATE%-%RANDOM%.txt
qmake -query QT_INSTALL_QML>%TEMPFILE%
set /p qt_install_qml=<%TEMPFILE%
del %TEMPFILE%
set qml_modules=(Qt QtQml QtQuick QtQuick.2 QtWebChannel QtWebEngine QtWebSockets QtWebView)
for %%m in %qml_modules% do (
	robocopy %qt_install_qml%\%%m %output_dir%\%%m /e
)

:: app resources
echo copy app resources..
robocopy %project_root_dir%\peregrine-desktop %output_dir% *.xml *.qml *.png *.svg

:: plugin binaries & resources
echo copy plugin binaries..
pushd %build_result_dir%\plugins
mkdir %output_dir%\plugins
for /d %%a in (*) do (
	mkdir %output_dir%\plugins\%%a\
	copy %%a\%optim_mode%\%%a.dll %output_dir%\plugins\%%a\
)

echo copy plugin resources
robocopy %project_root_dir%\plugins %output_dir%\plugins *.xml *.png *.svg *qml /S
popd

:EXIT_SUCCEEDED
endlocal
exit /b 0

:EXIT_FAILED
endlocal
exit /b 1