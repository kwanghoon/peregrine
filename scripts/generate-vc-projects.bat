setlocal

set path=%path%;%qt_dir%

pushd ..
	cd peregrine-desktop
	del peregrine-desktop.vcxproj
	del peregrine-desktop.vcxproj.filters
	qmake -tp vc peregrine-desktop.pro
	cd ..
	
	REM plugins
	cd plugins
	
	cd echo
	del echo.vcxproj
	del echo.vcxproj.filters
	qmake -tp vc echo.pro
	cd ..
	
	cd website
	del website.vcxproj
	del website.vcxproj.filters
	qmake -tp vc website.pro
	cd ..
	
	cd ..
popd

endlocal