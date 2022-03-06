import os, sys, shutil


dolphin_dir = sys.argv[1][ : sys.argv[1].rfind("\\")] # get dolphin dir from dolphin exe path

if not dolphin_dir:
    print("Invalid args")
    quit()


print("Zipping " + dolphin_dir)
root_dir = dolphin_dir[ : dolphin_dir.rfind("\\")]
base_dir = dolphin_dir[ dolphin_dir.rfind("\\")+1 : ]
shutil.make_archive("build_output", "zip", root_dir, base_dir)
print("Packaged successfully! See build_output.zip")