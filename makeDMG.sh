#!/usr/bin/env sh
rm -f /tmp/Myrmidon_20MB.dmg /tmp/Myrmidon.dmg ~/Documents/Myrmidon.dmg
hdiutil create -size 20m -fs HFS+ -volname Myrmidon -ov /tmp/Myrmidon_20MB.dmg
hdiutil mount /tmp/Myrmidon_20MB.dmg

cp -R myrmidon/myrmidon.app /Volumes/Myrmidon/myrmidon.app
ln -s /Applications /Volumes/Myrmidon/Applications

hdiutil unmount /Volumes/Myrmidon
hdiutil convert -format UDZO -o /tmp/Myrmidon.dmg /tmp/Myrmidon_20MB.dmg
hdiutil internet-enable -yes /tmp/Myrmidon.dmg

cp /tmp/Myrmidon.dmg ~/Documents/Myrmidon.dmg

