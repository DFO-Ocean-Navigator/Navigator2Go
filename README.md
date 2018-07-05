# Navigator2Go

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/32d9cc8cd9784783afc17c4e9bd3490e)](https://www.codacy.com/app/htmlboss/Ocean-Navigator-Config-Tool?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=DFO-Ocean-Navigator/Ocean-Navigator-Config-Tool&amp;utm_campaign=Badge_Grade)

Tool to manage your local Ocean Navigator installation.

## Intro
This tool is built in C++17 (mostly) with a Qt 5.11 UI. Qt Creator 4.6 is in a strange spot where it doesn't really "support" C++17, while GCC obviously does. The `CONFIG += c++1z` found in `Config_Tool.pro` is a temporary workaround.

### Features
* Control local gUnicorn and Apache Tomcat (THREDDS) servers.
* Apply the full power of the Ocean Navigator to your own data by importing as a netCDF.
* Download netCDF datasets from remote Navigator servers.
* Very high performance.
* Multi-threading where needed.

## Getting Started

For more details about the following build process go here: [http://doc.qt.io/qt-5/linux-building.html](http://doc.qt.io/qt-5/linux-building.html)

1. Install dependencies using the following:
```shell
sudo apt install build-essential valgrind libgl1-mesa-dev libfontconfig1-dev libfreetype6-dev libx11-dev libxext-dev libxfixes-dev libxi-dev libxrender-dev libxcb1-dev libx11-xcb-dev libxcb-glx0-dev libxcb-keysyms1-dev libxcb-image0-dev libxcb-shm0-dev libxcb-icccm4-dev libxcb-sync0-dev libxcb-xfixes0-dev libxcb-shape0-dev libxcb-randr0-dev libxcb-render-util0-dev
```
2. Go ahead and clone this repository into a directory of your choice:
```shell
git clone https://github.com/DFO-Ocean-Navigator/Ocean-Navigator-Config-Tool.git
```
3. Install Qt 5.11 Open Source: [https://www.qt.io/download](https://www.qt.io/download). Make sure you also check "Sources" when using the installer.
4. Navigate to the Qt installation directory.
5. Run `mkdir static`. Now we need to build Qt as a static library. This allows us to create a one-file executable.
6. `cd 5.11.0/Src/`
7. Run the following to configure Qt:
```shell
./configure -static -release -prefix $QT_INSTALL_DIRECTORY/static -skip qtdeclarative -skip qtremoteobjects -skip qtwebsockets -skip wayland -skip connectivity -skip multimedia -qt-xcb -nomake examples -nomake tests -opensource -confirm-license -no-feature-ftp -no-feature-pdf -no-feature-animation -no-feature-topleveldomain -no-feature-movie -no-feature-qt3d-extras -no-feature-qt3d-input -no-feature-qt3d-logic -no-feature-qt3d-opengl-renderer -no-feature-qt3d-render -no-feature-qt3d-simd-avx2 -no-feature-qt3d-simd-sse2  -skip location -no-accessibility -no-openssl -no-mips_dsp -no-mips_dspr2 
```
8. Then run: `make -j12 clean`. The `-j12` tells make how many jobs it should spawn to build the project. Normally the number specified is the number of physical cores in your system.
9. Assuming no errors occoured (which shouldn't), finally run `make -j12 install`.
10. Et Voila! Vous avez terminé!

## Building for Release
So, you've made some changes to my lovely code that made it even more awesome. Time to send it out to the masses.
1. Navigate to the source folder `Ocean-Navigator-Config-Tool/Config_Tool/`.
2. Run the following: `$QT_INSTALL_DIRECTORY/static/bin/qmake -config release`.
3. `make -j12 clean`.
4. `make -j12`.
5. You can now run the tool like so `./Config_Tool`.