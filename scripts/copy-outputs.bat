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
timeout 1
mkdir %output_dir%

:: app binaries & QT dlls
echo copy app binaries..
copy %build_result_dir%\peregrine-desktop\%optim_mode%\peregrine-desktop.exe %output_dir%

if "%qt_dir%" == "" (
	echo not found 'qt_dir' environment variable.
	goto EXIT_FAILED
)
set qt_modules=(Core Widgets Gui Qml Quick QuickWidgets Xml Network)
if "%optim_mode%" == "debug" (
	set d_suffix=d
)
for %%m in %qt_modules% do (	
	copy "%qt_dir%\Qt5%%m%d_suffix%.dll" %output_dir%
)

:: app resources
echo copy app resources..
robocopy %project_root_dir%\peregrine-desktop %output_dir% *.xml *.qml

:: plugin binaries & resources
echo copy plugin binaries..
pushd %build_result_dir%\plugins
mkdir %output_dir%\plugins
for /d %%a in (*) do (
	mkdir %output_dir%\plugins\%%a\
	copy %%a\%optim_mode%\%%a.dll %output_dir%\plugins\%%a\
)

echo copy plugin resources
robocopy %project_root_dir%\plugins %output_dir%\plugins *.xml *.png *qml /S
popd

:EXIT_SUCCEEDED
endlocal
exit /b 0

:EXIT_FAILED
endlocal
exit /b 1