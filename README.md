# Cosmic Capture

# Development Setup
0. Before you start, ensure that you have compiled physx following the instructions in tutorial 1, and that you have installed everything covered in tutorial 0. You will need the .dll and .lib files that the compiler will generate.

1. Clone the repository.

2. Copy Files and Create Directories.
- Open your solution directory, and create a `lib` folder and a `x64` folder. Move into the `lib` folder, and create two folders: `Debug` and `Release`. Also create `Debug` and `Release` folders in the `x64 folder`.
- Move all physx .lib files located in `physx\bin\win.x86_64.vc142.mt\debug` into the `lib\Debug` folder you created. Move the .lib files located in the release folder into the `lib/Release` folder you created.
- Move all physx .dll files located in `physx\bin\win.x86_64.vc142.mt\debug` into the `x64\Debug` folder you created. Move the .dll files located in the release folder into the into the `x64\Release` folder you created.

3. Check that everything is working by compiling and running in both debug and release mode. You should see no errors.
