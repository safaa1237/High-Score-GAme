How to use the benchmark:
=========================

- Compile and place the EXE and all neceesary DLLs in a new folder
- Create a new sub folder named "RawScans"
- Copy some raw scans into the "RawScans" folder
  (scans should work with automatic top view + c2c registration)
- Run the executable
- After a successful run, the benchmark will save the results to a JSON file


Command line parameters/argument:
=================================

Start the benchmark executable with --help to list all optional settings.
The list includes a description and the default value for each option.


How to use with a specific API version:
=======================================

If you want to execute the benchmark with a specific older version of the SCENE
API runtime, you have to make sure that no newer version is installed.

There are two different benchmark executables, one that requires the DLL dependencies
in the same folder and another that can use the WinSxS/SideBySide mechanism of windows.

They are called SceneBenchmark.exe and SceneBenchmarkWinSxS.exe.
You need the second one to use an installed SCENE API runtime!

The benchmark will then automatically use the latest/newest installed version of the
SCENE API Redistributable runtime. The oldest supported SCENE API Redistributable
runtime is 2.0.14.