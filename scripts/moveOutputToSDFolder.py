import os
import shutil

current_dir = os.getcwd() + "\\"

ProjectPlus = current_dir + "SDCard\\Project+\\codes\\"
vBrawl = current_dir + "SDCard\\vBrawl\\codes\\"
PM = current_dir + "SDCard\\projectm\\codes\\"
games = [ProjectPlus, vBrawl]



buildOutput = current_dir + "build\\Output\\"

def transfer_files(dest_folder):
    for file in os.listdir(buildOutput):
        try:
            shutil.copy(f"{buildOutput}\\{file}", f"{dest_folder}\\{file}")
        except Exception as e:
            print(e)

for game in games:
    transfer_files(game)

print("Copied files successfully!")