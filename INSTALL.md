# Qt with Qt Creator (Recommended)

0. Follow the instructions on BrightSpace: Content - Information - Assignment.
1. Open the `CMakeLists.txt` file using `Open File or Project` menu or `Open Project` button.
2. From the list of environments select any (preferably `x64`) `Desktop` environment. If no such environment is listed, check and install necessary development environment. On Windows this can be installed together with Microsoft Visual Studio (Full or Express version, but not to be confused with Visual Studio Code with is completely separate software). Alternatively, during the installation of Qt with Open Source license, MinGW can be installed, allowing Windows to access some Linux tools like C++ compiler. On Mac, the C/C++ compilers are usually installed alongside XCode. On Linux, the C/C++ compilers `gcc` or `clang` are available directly from the distribution package system.
3. Press `Configure Project` button and wait for automatic configuration to complete. If it does not produce any errors, the project is installed correctly, and can be build and run from within Qt Creator.

# Qt with CMake

The purpose of this project is to provide Qt6 build of the framework, without the need for `QtCreator`.
An independent `QtDesigner` can be used for modifying the `*.ui` files.

Environment required (untested = might not work, require additional setup):

- C/C++ compiler: GCC, Clang;
- An IDE: VSCode (potentially CLion, MS Visual Studio, etc);
- CMake: Used for building the project - generate project files or make files.

Depending on the IDE used knowledge of bash might be required.

Assuming Qt6 is installed and the current command-line directory is in the `src` directory:

1. `mkdir build`
    - creates a build directory;
2. `cd build`
    - change the current directory to the build;
3. `cmake ..`
    - if Qt6 is installed correctly and found in the default search directory. An error might occur otherwise.
4. `make`
    - compiles the project.
5. `OpenGL_0`
    - executes the compiled program.

Alternatively, `QtCreator` (and probably most other IDEs) allows you to directly compile and run the program.

# Versions

Qt6 is supported on Windows, macOS, and Linux using the installers found at <https://download.qt.io/official_releases/online_installers/>. Registration is required, but only an email address is required to register (no private data or payment is required; the phone number field can be left blank). ~5GiB of download is required. Additionally, Qt6 is also available in the Linux packages `qt6-base-dev` and `qt6-3d-dev`.

Depending on your system, a Linux machine may require you to install graphics drivers in order to run OpenGL. The open-source Mesa drivers should be automatically offered in Ubuntu 18.04 and up. You can use the `glxinfo` command found in the `mesa-utils` package to check whether the drivers are installed and up to date:

```bash
glxinfo | grep Mesa
```

If this is not the case, make sure to install the following two packages:

- mesa-utils
- libgl1-mesa-dev

In case you encounter issues setting up Qt, please contact a TA via the Computer Graphics helpdesk or in a lab session.

When installing Qt, you may notice that the installation size is rather large by default (upwards of 30GB). In order to reduce this, most components can be disabled. For example, on Windows, the following components are sufficient to complete the OpenGL assignments in Qt Creator, and add up to a much smaller size of less than 5GB:

- Qt:
    - Qt (version 6.2 or above):
        - MinGW components (replace with MSVC if needed)
- Developer and Designer Tools:
    - Qt Creator
    - Qt Creator CDB Debugger Support
    - Debugging Tools for Windows
    - MinGW
    - CMake
    - Ninja

> Important: if you use the installer, you may have to set the environment variable `CMAKE_PREFIX_PATH` so that Qt will be able to locate the correct cmake executable. On linux, this may look something like this: `export CMAKE_PREFIX_PATH=/home/usr/Qt/6.4.2/gcc_64/lib/cmake` (the path may vary slightly depending on the version).
