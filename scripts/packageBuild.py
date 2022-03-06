import os, sys, shutil


dolphin_dir = sys.argv[1][ : sys.argv[1].rfind("\\")] # get dolphin dir from dolphin exe path

if not dolphin_dir:
    print("Invalid args")
    quit()

# copy P+ launcher elf into dolphin folder
pplus_launcher_elf = sys.argv[2] + "/Project+.elf"
shutil.copy(pplus_launcher_elf, dolphin_dir)

print("Zipping " + dolphin_dir)
root_dir = dolphin_dir[ : dolphin_dir.rfind("\\")]
base_dir = dolphin_dir[ dolphin_dir.rfind("\\")+1 : ]
shutil.make_archive("build_output", "zip", root_dir, base_dir)
print("Packaged successfully! See build_output.zip")