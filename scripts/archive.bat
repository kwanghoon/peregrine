@echo off

setlocal

:: checking the prerequites.
if "%1" == "" (
	echo no target directory specified.
	exit /b 1
) else (
	set target_dir=%1
)

where tar
if %errorlevel% NEQ 0 (
	echo no existing tar command.
	exit /b 1
)
where gawk
if %errorlevel% NEQ 0 (
	echo no gawk command.
	exit /b 1
)

:: packing into .tar
echo packing into .tar
tar -cvf ../output.tar ../output

:: get SHA1 of current HEAD' as variable.
echo getting SHA1
git show --oneline | gawk "{ if (NR == 1) { print $1 } }" > sha1.txt
set /p sha1=<sha1.txt
del sha1.txt

:: copy
echo copying .tar with rename
git show | gawk "{ if (NR == 3) { gsub(\":\", \".\", $5); print $5 } }" > author_time.txt
set /p author_time=<author_time.txt
del author_time.txt
set filename=[%date%][%author_time%]%sha1%.tar
move ..\output.tar "%target_dir%\%filename%"

endlocal