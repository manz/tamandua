#!/usr/bin/env sh

# teste la non existence du Makefile
if [ ! -e Makefile ];
then
	cmake -DCMAKE_BUILD_TYPE=Debug .
fi;

make

PLUGINPATH="myrmidon/myrmidon.app/Contents/PlugIns"

if [ ! -d $PLUGINPATH ];
then
	mkdir $PLUGINPATH
fi;

cp *.so $PLUGINPATH

FRAMEPATH="myrmidon/myrmidon.app/Contents/Frameworks"

if [ ! -d $FRAMEPATH ];
then
	mkdir $FRAMEPATH
fi;

if [ ! -d $FRAMEPATH/QtGui.framework ];
then
	cp -R /usr/lib/qt4/lib/QtGui.framework $FRAMEPATH
fi;

if [ ! -d $FRAMEPATH/QtCore.framework ];
then
	cp -R /usr/lib/qt4/lib/QtCore.framework $FRAMEPATH
fi;

cp libtd_base.dylib $FRAMEPATH
cp libtd_core.dylib $FRAMEPATH

APP=myrmidon/myrmidon.app/Contents

install_name_tool -id @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
           $APP/Frameworks/QtCore.framework/Versions/4/QtCore

install_name_tool -id @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
					 $APP/Frameworks/QtGui.framework/Versions/4/QtGui

install_name_tool -id @executable_path/../Frameworks/libtd_core.dylib \
										$APP/Frameworks/libtd_core.dylib

install_name_tool -id @executable_path/../Frameworks/libtd_base.dylib \
										$APP/Frameworks/libtd_base.dylib

install_name_tool -change /usr/lib/qt4/lib/QtCore.framework/Versions/4/QtCore \
										@executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
										$APP/MacOS/myrmidon

install_name_tool -change /usr/lib/qt4/lib/QtGui.framework/Versions/4/QtGui \
										@executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
										$APP/MacOS/myrmidon

install_name_tool -change /usr/lib/qt4/lib/QtCore.framework/Versions/4/QtCore \
										@executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
					 					$APP/Frameworks/QtGui.framework/Versions/4/QtGui

install_name_tool -change `pwd`/libtd_base.dylib \
										@executable_path/../Frameworks/libtd_base.dylib \
										$APP/MacOS/myrmidon

install_name_tool -change `pwd`/libtd_core.dylib \
										@executable_path/../Frameworks/libtd_core.dylib \
										$APP/MacOS/myrmidon

install_name_tool -change `pwd`/libtd_base.dylib \
										@executable_path/../Frameworks/libtd_base.dylib \
										$APP/Frameworks/libtd_core.dylib


