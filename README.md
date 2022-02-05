
Based off framework by Fracture and Fudgepop01  
https://github.com/Fracture17/ProjectMCodes  

This mod is to be used in conjunction with this [Brawlback dolphin fork](https://github.com/FaultyPine/dolphin).  

# Building

Firstly, the `pf` folder in p+/pm is way too big for git, so it's gitignored. You'll have to provide your own for testing those mods.  
  
First, you'll need to install some tools that will allow you to build the C++ mod files.  
Follow the guide linked below to get that set up.  
The guide just runs you through installing git, mingw, vscode, some cmake tools for vscode, and getting it all set up for building
the mod.  
  
https://www.youtube.com/watch?v=oGg2dgYN1Do  
  
Once you are able to build mod files like the tutorial above shows follow the next step.  
  
To set up the build process, change the paths in `build_settings.ini` found in the root of this project, and the `Config.ini` file found in `scripts/VSDSync`. If you have any questions about what the path variables mean, let me know. I am more than happy to help get things set up for you.  
  
  
That should be it, if you're trying to get this building and something isn't working, let me know, or ask for help in the [Brawlback discord server](https://discord.gg/dzYRN32k4D)  