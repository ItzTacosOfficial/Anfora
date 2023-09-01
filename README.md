# Anfora
### Modloader attempt for the 2007 PC Game Crazy Machines 2

## Building

### Requirements
- [CM2 Reference](https://github.com/ItzTacosOfficial/CM2-Reference)
- [Microsoft Detours](https://github.com/microsoft/Detours/tree/4.0.1)
- CMake
###### [JetBrains's CLion](https://www.jetbrains.com/clion/) is recommended

### Preparation
Download the CM2 Reference repository, complete its requirements and the preparation steps.

Add the root of the reference repository to a new system enviroment variable named `CM2_ROOT`.

Download the Microsoft Detours repository and build the library for 32 bits by following guide in the README.

Copy Microsoft Detours's headers into `lib/detours/include` and the built library into `lib/detours/x86`.

### Compiling
Build first the loader (`anforaloder.dll`) CMake target and copy the output `anforaloader.lib` file into `lib/anforaloader/x86` (If you want you can automate the process with a CMake post build custom command).

Then build the launcher (`anfora.exe`) CMake target.
