import os
import shutil

current_dir = os.getcwd() + "\\"

ProjectPlus = current_dir + "SDCard\\Project+\\codes\\"
vBrawl = current_dir + "SDCard\\vBrawl\\codes\\"

buildOutput = current_dir + "build\\Output\\"

def transfer_files(dest_folder):
    for file in os.listdir(buildOutput):
        try:
            shutil.copy(f"{buildOutput}\\{file}", f"{dest_folder}\\{file}")
            #print("Adding", file)
        except Exception as e:
            print(e)

transfer_files(ProjectPlus)
transfer_files(vBrawl)

print("Copied files successfully!")