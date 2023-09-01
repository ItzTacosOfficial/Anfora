# Anfora
### Modloader *attempt* for the 2007 PC Game Crazy Machines 2

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
Build first the loader (`anforaloader`) target and copy the output `anforaloader.lib` file into `lib/anforaloader/x86` (If you want you can automate the process with a CMake post build custom command).

Then build the launcher (`anfora`) target.

## Building a mod
Building a mod follows the same process as building the modloader.

Refer to the CMake `example-mod` target and to the source `src/example-mod/mod.cpp` for API documentation.

Once your mod is built, copy it into the `mods` folder (gets created after first `anfora.exe` startup).

## Important notes
This modloader is not meant to be stable, it was born as a tool to help reverse engineering the game with a single mod loaded.

Currently common things such has multiple mods hooking into the same place and probably other things I am not aware of are undefined.

The state of the project might change later on, but as of now I am doing it out of fun without the objective of being that usable to users.
