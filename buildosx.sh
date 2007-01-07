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
