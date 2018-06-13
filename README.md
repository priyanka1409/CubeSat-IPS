# CubeSat-IPS
Image Processing software code for the CubeSat Galassia II

## Major libraries used in the project:
- OpenCV 3.4.1
- GDAL 2.2.2

## Prerequisites:
- Windows 10, 8.1, 7, Linux, or MacOS
- Visual Studio 2017 or Visual Studio 2015 Update 3 (on Windows)
- Git
- Optional: CMake 3.10.2

## Installing libraries
To get started:
```
> git clone https://github.com/Microsoft/vcpkg
> cd vcpkg

PS> .\bootstrap-vcpkg.bat
Ubuntu:~/$ ./bootstrap-vcpkg.sh
```

Then, to hook up user-wide [integration](docs/users/integration.md), run (note: requires admin on first use)
```
PS> .\vcpkg integrate install
Ubuntu:~/$ ./vcpkg integrate install
```
To install the libraries, install via packages with
```
PS> .\vcpkg install opencv:x64-windows
Ubuntu:~/$ ./vcpkg install opencv:x64-windows

PS> .\vcpkg install gdal:x64-windows
Ubuntu:~/$ ./vcpkg install gdal:x64-windows
```

Once these commands are implemented, all installed libraries are immediately ready to be #include'd into the Visual Studio project and used without additional configuration.
