# Ocean-Navigator-Config-Tool
Tool to help with customising the Navigator's config files.

## Intro
The config tool is built in C++17 with a Qt 5.11 UI. This wiki page will list how to make changes to the code, and subsequently re-build a standalone executable for Linux.

For more details about the following build process go here: [http://doc.qt.io/qt-5/linux-building.html](http://doc.qt.io/qt-5/linux-building.html)

## Getting Started
1. Install dependencies using the following:
```shell
sudo apt install build-essential libgl1-mesa-dev libfontconfig1-dev libfreetype6-dev libx11-dev libxext-dev libxfixes-dev libxi-dev libxrender-dev libxcb1-dev libx11-xcb-dev libxcb-glx0-dev libxcb-keysyms1-dev libxcb-image0-dev libxcb-shm0-dev libxcb-icccm4-dev libxcb-sync0-dev libxcb-xfixes0-dev libxcb-shape0-dev libxcb-randr0-dev libxcb-render-util0-dev
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
3. `make clean`.
4. `make`.
5. You can now run the tool like so `./Config_Tool`.