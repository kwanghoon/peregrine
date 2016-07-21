export QMAKE=~/Qt5.7.0/5.7/gcc_64/bin/qmake

pushd ..

mkdir build-peregrine-desktop
cd build-peregrine-desktop
$QMAKE ../peregrine-desktop/peregrine-desktop.pro
make
cd ..

popd
 
