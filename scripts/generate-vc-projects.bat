setlocal

set path=%path%;%qt_dir%

pushd ..
	cd peregrine-desktop

	del peregrine-desktop.vcxproj
	del peregrine-desktop.vcxproj.filters

	qmake -tp vc peregrine-desktop.pro
	cd ..
popd

endlocal