# bin/sh

# Setup configuration and paths
source ./config_osx.sh

# Build codes
cmake --build ./build --config Debug --target all -j 14 --

# Mount SD (Make sure to run build_sdcard[osx].sh first!)
hdiutil attach -imagekey diskimage-class=CRawDiskImage "$SDPATH"

# Move output code/binaries to SD folder
python3 scripts/moveOutputToSDFolderOSX.py

# Copy all from SD folder to the mounted SD Card
cp -R ./SDCARD/ /Volumes/$SDNAME/

