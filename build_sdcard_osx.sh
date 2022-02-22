# bin/sh

# Setup configuration and paths
source ./config_osx.sh

# Create the SD CARD
hdiutil create -megabytes 2048 -fs MS-DOS -layout NONE -volname $SDNAME -ov -o sd.raw

# Rename the .dmg to .raw
mv sd.raw.dmg sd.raw

# Move it to Dolphin shared path
mv -f sd.raw "$SDPATH"

echo "Created $SDPATH with Volname $SDNAME"
