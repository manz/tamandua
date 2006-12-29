# teste la non existence du Makefile
if [ ! -e Makefile ];
then
	cmake .
fi;

make

PLUGINPATH="myrmidon.app/Contents/PlugIns"

if [ ! -d $PLUGINPATH ];
then
	mkdir myrmidon.app/Contents/PlugIns
fi;

cp *.so myrmidon.app/Contents/PlugIns
