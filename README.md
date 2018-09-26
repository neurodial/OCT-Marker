# OCT-Marker

a platform independent open source tool for open OCT images and create different labels on it

  * quality intervals
  * free form segmentation
  * edit layer segmentation
  * mark ONH in SLO image with rectangle
  * rectangles on bscans
  * classify bscans and volumes

## License

This projekt is licensed under the GPL3 License - see [license.txt](license.txt) file for details

## Build OCT-Marker

### On Windows - setup build enviroment

install

  * a **C++ compiler**, as example MSVC

    [Visual Studio 2017 build tools](https://aka.ms/vs/15/release/vs_buildtools.exe)

    choose: c++ Build tools

  * **Qt 5**

    [Qt Download](https://www.qt.io/download-qt-installer)

    install the newest version for you compiler, for the above set the checkbox only for VC 2017 15

  * **CMake**

    [CMake download](https://cmake.org/download/)

  * **Boost C++ Libraries**

    [Boost C++ Libraries download](https://www.boost.org/users/download/)

    build boost on visual studio promt with
```
call bootstrap.bat
b2 -j8 toolset=msvc-14.1 address-model=64 architecture=x86 link=static threading=multi runtime-link=shared --build-type=complete stage 
```

  * **OpenCV**

    [OpenCV download](https://opencv.org/releases.html)

    Build OpenCV with (git-bash recommend)
```
mkdir build && cd build
cmake -G "Visual Studio 15 2017 Win64" ..
cmake --build . --config Release
```

  * **Zlib**

    [Zlib download](https://zlib.net/)
    
    build with cmake simply

  * **DCMTK** (for experimentell Cirrus support)
  * **OpenJPEG** (for Topcon and experimental Cirrus support)
 
### On Linux - setup build environment
Install from distribution repository

   * the standard c++ compiler (mostly the g++)
   * cmake

and the develop packages of

   * Qt5
   * OpenCV
   * Boost
   * Zlib
   * libtiff (for reading tiffstacks)
   * DCMTK (for experimental Cirrus reader)
   * OpenJPEG (for Topcon and experimental Cirrus reader)

### Build

Get the projects

  * oct_cpp_framework
  * LibE2E (for reverse engineered E2E support)
  * LibOctData
  * Oct-Marker

Build all projekts in the above order on Windows with (git-bash recommend)
```
mkdir build && cd build
cmake -G "Visual Studio 15 2017 Win64" ..
cmake --build . --config Release
```

Build all projekts in the above order on Linux with
```
mkdir build && cd build
cmake  -DCMAKE_BUILD_TYPE=Release ..
make
```
