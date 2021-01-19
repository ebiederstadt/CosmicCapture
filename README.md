# Cosmic Capture

## Development Setup
0. Before you start, ensure that you have compiled physx following the instructions in tutorial 1, and that you have installed everything covered in tutorial 0. You will need the .dll and .lib files that the compiler will generate.

1. Clone the repository.

2. Setup Visual Studio.
- In Project Properties > VC++ Directories > Include Directories, add `$(SolutionDir)include`. Repeat in both Debug and Release mode.
- In Project Properties > VC++ Directories > Library Directories, add `$(SolutionDir)lib\Debug` (for debug) and `$(SolutionDir)lib\Release` (for release).
- In Project Properites > Linker > Input, add the following additional dependencies for both release and debug mode.
```
PhysX_64.lib
PhysXCommon_64.lib
PhysXCooking_64.lib
PhysXExtensions_static_64.lib
PhysXFoundation_64.lib
PhysXTask_static_64.lib
SDL2.lib
SDL2main.lib
```
- In Project Properites > Configuration Properties > General > Output Directory, add `$(SolutionDir)x64\$(Configuration)\` for both release and debug modes.
- Make sure that you are targeting `x64` when compiling for both debug and release modes.

3. Copy Files and Create Directories. Note that our google drive has a folder containing all the .dll and .lib files you need. The Files needed for SDL will work for sure, but the files needed for physx were compiled on my machine and may not work with your machine.
- Open your solution directory, and create a `lib` folder and a `x64` folder. Move into the `lib` folder, and create two folders: `Debug` and `Release`. Also create `Debug` and `Release` folders in the `x64 folder`.
- Move all physx .lib files located in `physx\bin\win.x86_64.vc142.mt\debug` into the `lib\Debug` folder you created. Move the .lib files located in the release folder into the `lib\Release` folder you created.
- Move all physx .dll files located in `physx\bin\win.x86_64.vc142.mt\debug` into the `x64\Debug` folder you created. Move the .dll files located in the release folder into the into the `x64\Release` folder you created.
- Move all SDL .lib files into `lib\Debug` and `lib\Release`. Move `SDL2.dll` into `x64\Debug` and `x64\Release`. The files you need are on the google drive. Repeat this process for GLEW lib and dll files.

3. Check that everything is working by compiling and running in both debug and release mode. You should see no errors.
