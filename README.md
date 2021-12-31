
Based off framework by Fracture and Fudgepop01
https://github.com/Fracture17/ProjectMCodes



# Building

Firstly, the `pf` folder in p+/pm is way too big for git, so it's gitignored. You'll have to provide your own for testing
those mods.  

Next, there are some files that have hardcoded paths. Look for the comment `(CHANGEME)`.
Change all the relevant path variables in the `.bat` scripts in this project's root directory.
Also, in `scripts/VSDSync/Config.ini`, you'll want to change `BUILD_DIR` to point to this project folder, `SD_CARD_PATH` to point to your sd.raw, and `DOLPHIN_PATH` to point to your dolphin executable
    - alternatively, just run the exe at `scripts/VSDSync/Setup.exe` to set that up with a GUI


See build.bat comment for info on how to use the build script

That should be it, if you're trying to get this building and something isn't working, let me know.