# Navigator2Go

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/32d9cc8cd9784783afc17c4e9bd3490e)](https://www.codacy.com/app/htmlboss/Ocean-Navigator-Config-Tool?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=DFO-Ocean-Navigator/Ocean-Navigator-Config-Tool&amp;utm_campaign=Badge_Grade)

Tool to manage your local Ocean Navigator installation.

### Features
* Control local gUnicorn and Apache Tomcat (THREDDS) servers.
* Apply the full power of the Ocean Navigator to your own data by importing as a netCDF.
* Customize your Navigator with easy-to-use configuration editors.
* Download netCDF datasets from remote Navigator servers.
* Very high performance.
* Schedule jobs to update your data.
* Multi-threading where needed.

## Getting Started

For more details about the following build process go here: [http://doc.qt.io/qt-5/linux-building.html](http://doc.qt.io/qt-5/linux-building.html)

1. Install GCC 7 or newer to get access to all the latest and greatest C++ 17 features.
2. Install dependencies using the following:
```shell
sudo apt install build-essential valgrind zlib1g-dev libgl1-mesa-dev libfontconfig1-dev libfreetype6-dev libx11-dev libxext-dev libxfixes-dev libxi-dev libxrender-dev libxcb1-dev libx11-xcb-dev libxcb-glx0-dev libxcb-keysyms1-dev libxcb-image0-dev libxcb-shm0-dev libxcb-icccm4-dev libxcb-sync0-dev libxcb-xfixes0-dev libxcb-shape0-dev libxcb-randr0-dev libxcb-render-util0-dev
```
3. Go ahead and clone this repository into a directory of your choice:
```shell
git clone https://github.com/DFO-Ocean-Navigator/Navigator2Go.git
```
4. Install Qt 5.11 Open Source: [https://www.qt.io/download](https://www.qt.io/download). Make sure you also check "Sources" when using the installer.
5. Navigate to the Qt installation directory.
6. Run `mkdir static`. Now we need to build Qt as a static library. This allows us to create a one-file executable.
7. `cd 5.11.0/Src/`
8. Run the following to configure Qt:
```shell
./configure -static -release -prefix $QT_INSTALL_DIRECTORY/static -skip qtdeclarative -skip qtremoteobjects -skip qtwebsockets -skip wayland -skip connectivity -skip multimedia -qt-xcb -nomake examples -nomake tests -opensource -confirm-license -no-feature-ftp -no-feature-pdf -no-feature-animation -no-feature-topleveldomain -no-feature-movie -no-feature-qt3d-extras -no-feature-qt3d-input -no-feature-qt3d-logic -no-feature-qt3d-opengl-renderer -no-feature-qt3d-render -no-feature-qt3d-simd-avx2 -no-feature-qt3d-simd-sse2  -skip location -no-accessibility -no-openssl -no-mips_dsp -no-mips_dspr2 
```
9. Then run: `make -j12 clean`. The `-j12` tells make how many jobs it should spawn to build the project. Normally the number specified is the number of physical cores in your system.
10. Assuming no errors occoured (which shouldn't), finally run `make -j12 install`.
11. Et Voila! Vous avez terminé!

## Building for Release
So, you've made some changes to my lovely code that made it even more awesome. Time to send it out to the masses.
1. Navigate to the source folder `Navigator2Go/Config_Tool/`.
2. Run the following: `$QT_INSTALL_DIRECTORY/static/bin/qmake -config release`.
3. `make -j12 clean`.
4. `make -j12`.
5. Fix any compile errors that crop up.
6. You can now run the tool like so `./Config_Tool`.

## Incrementing Version Number
The program's version number is stored in `VERSION.txt`. This is included in `Config_Tool.pro` to make the number accessible to `qmake` and to C++.
After changing the number as needed:
* Run qmake from Qt Creator.
* Clean and re-build the project.
* Run the output binary and verify the number by going to Help -> About.
