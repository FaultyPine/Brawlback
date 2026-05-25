import os, ntpath, re
from Library import File, Library
from Section import Section
from Common import cmdLineOutput, makeBinUtilCommandFile, makeCustomLinkerScriptFile
from typing import List
ppcCompiler = R'C:\Users\dareb\Documents\PPlusCpp\BuildSystem\Compiler\bin\powerpc-eabi-g++.exe'
linkerScriptBase = R'C:\Users\dareb\Documents\PPlusCpp\BuildSystem\src\customLinkerScript.ld'
binUtilCommandFilePath = 'IntermediateFiles\\binUtilCommands.txt'

class Compiler:
    STANDARD_OPTIONS = [
     '-ggdb3', '-nostartfiles', '-fomit-frame-pointer', '-fno-function-cse', '-ffunction-sections', '-fdata-sections',
     '-fno-exceptions', '-fno-rtti', '-fno-asynchronous-unwind-tables', '-fno-unwind-tables', '-fno-stack-check', '-std=c++17',
     '-fno-builtin', '-ffreestanding', '-mcpu=750', '-mmultiple', '-fno-inline', '-save-temps=obj', '-fno-eliminate-unused-debug-symbols', '-fno-eliminate-unused-debug-types',
     '-fverbose-asm', '-fno-threadsafe-statics', '-z common-page-size=4', '-z max-page-size=4',
     '-Wl,"--relax"', '-Wl,"--gc-sections"']

    def __init__(self, options: list=None):
        if options is None:
            options = self.STANDARD_OPTIONS
        self.options = options

    def compile(self, cppFile: File, libraries, textStart: int=None, dataStart: int=None, sections: List[Section]=None, outPath: str=None, extraOptions: list=None):
        print(cppFile.path)
        if not cppFile.exists():
            raise AssertionError(f"{cppFile.path} not found")
        else:
            options = self.options.copy()
            customSections: list[str] = []
            if textStart is not None:
                options.append(f'-Wl,"-Ttext={hex(textStart)}"')
            if dataStart is not None:
                options.append(f'-Wl,"--section-start=.rodata={hex(dataStart)}"')
            if sections is not None:
                for s in sections:
                    if cppFile.name == "initFile.cpp":
                        # customSections.append(f'. = SEGMENT_START("{s.name}", {hex(s.address)});')
                        customSections.append(f'{s.name} {hex(s.address)} : ' + "{ }")
                    else:
                        options.append(f'-Wl,"--section-start={s.name}={hex(s.address)}"')

        if extraOptions is not None:
            options.extend(extraOptions)
        
        
        with open(linkerScriptBase) as file:
            linkerScript = file.read()
            
        linkerScript = makeCustomLinkerScriptFile(linkerScript.replace("[REPLACE_ME_WITH_STUFFS]", '\n\t'.join(customSections)))
        commandFile = makeBinUtilCommandFile(' '.join(options))
        if outPath is None:
            outPath = ntpath.splitext(cppFile.path)[0]
        libraries = ' '.join([lib.path for lib in libraries])
        if cppFile.name == "initFile.cpp":
            compileCommand = f"{ppcCompiler} {cppFile.path} -T{linkerScript.path} @{commandFile.path} {libraries} {libraries} {libraries} -o {outPath}"
        else:
            compileCommand = f"{ppcCompiler} {cppFile.path} @{commandFile.path} {libraries} {libraries} {libraries} -o {outPath}"
            
        try:
            print(">>> STARTING")
            output = cmdLineOutput(compileCommand)
            output = filterUselessWarnings(output)
            # print(output)
            # if output:
                # print(output)
        except Exception as e:
            print(">>> SOME EXCEPTION OCCURED??")
            print(["error", e])
            paths = compileCommand.split(" ")
            for path in paths:
                print(f"testing path: {path}...")
                if not os.path.exists(path):
                    print(f"failed to get path {path}")
            # print(f"??? ==\n{compileCommand}\n== ???")
            os.system(compileCommand)
        else:
            print(">>> OUTPUT OK\n\n\n\n\n")
            return Library(outPath)


def filterUselessWarnings(output):
    output = re.sub('.*warning: cannot find entry symbol _start.*', '', output)
    output = re.sub('.*warning: dot moved backwards before.*', '', output)
    output = output.strip()
    return output