::
:: copy build results to the output directory.
::
:: arguments
::   %1 - compile optimization mode. debug or release.
::
@echo off
setlocal

if not exist "%~dp0..\build-peregrine" (
	echo "Not build results found! (root/build-peregrine)"
	goto EXIT_FAILED
)

set optim_mode=%1
if "%optim_mode%" == "" set optim_mode=debug
set project_root_dir=%~dp0..
set build_result_dir=%project_root_dir%\build-peregrine
set output_dir=%project_root_dir%\output

rmdir /s/q %output_dir%
mkdir %output_dir%

:: app binaries
echo copy app binaries..
copy %build_result_dir%\peregrine-desktop\%optim_mode%\peregrine-desktop.exe %output_dir%
copy %project_root_dir%\peregrine-desktop\*.xml %output_dir%

:: plugin binaries & resources
echo copy plugin binaries..
pushd %build_result_dir%\plugins
mkdir %output_dir%\plugins
for /d %%a in (*) do (
	mkdir %output_dir%\plugins\%%a\
	copy %%a\%optim_mode%\%%a.dll %output_dir%\plugins\%%a\
)

echo copy plugin resources
robocopy %project_root_dir%\plugins %output_dir%\plugins *.xml *.png /S
popd

:EXIT_SUCCEEDED
endlocal
exit /b 0

:EXIT_FAILED
endlocal
exit /b 1