setlocal

call "%vs140comntools%vsvars32.bat"
set path=%path%;%qt_dir%

pushd ..
	set project_root=%cd%
	
	::
	echo peregrine-desktop
	rmdir /s/q build-peregrine-desktop
	mkdir build-peregrine-desktop
	pushd build-peregrine-desktop
		qmake -spec win32-msvc2015 ..\peregrine-desktop\peregrine-desktop.pro
		nmake
	popd
	
	::
	echo copy outputs
	rmdir /s/q output
	mkdir output
	pushd output
		copy %project_root%\build-peregrine-desktop\release\peregrine-desktop.exe .
		copy "%qt_dir%\Qt5Core.dll" .
		copy "%qt_dir%\Qt5Widgets.dll" .
		copy "%qt_dir%\Qt5Gui.dll" .
	popd

popd

endlocal
