@echo off
setlocal

set path=%path%;%qt_dir%

pushd ..
	qmake -tp vc -r peregrine.pro
popd

endlocal