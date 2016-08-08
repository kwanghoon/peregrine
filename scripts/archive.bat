@echo off

setlocal

:: checking the prerequites.
if "%1" == "" (
	echo no target directory specified.
	exit /b 1
) else (
	set target_dir=%1
)
mkdir %target_dir%

where 7z
if %errorlevel% NEQ 0 (
	echo no existing 7z command.
	exit /b 1
)
where gawk
if %errorlevel% NEQ 0 (
	echo no gawk command.
	exit /b 1
)

:: packing into .7z
echo packing into .7z
set temp7z=%TEMP%\TEMP-%DATE%-%RANDOM%.7z
7z a %temp7z% ../output

:: get SHA1 of current HEAD' as variable.
echo getting SHA1
SET temptxt=%TEMP%\TEMP-%DATE%-%RANDOM%.txt
git show --oneline | gawk "{ if (NR == 1) { print $1 } }" > %temptxt%
set /p sha1=<%temptxt%

:: copy
echo copying .7z with rename
git show | gawk "{ if (NR == 3) { gsub(\":\", \".\", $5); print $5 } }" > %temptxt%
set /p author_time=<%temptxt%
set filename=[%date%][%author_time%]%sha1%.7z
if exist "%target_dir%\%filename%" del "%target_dir%\%filename%"
move %temp7z% "%target_dir%\%filename%"
del %temptxt%

endlocal