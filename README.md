# ECL IDE

##  Building on windows

### Prerequisits 
* git (optional - needed to clone sources)
* cmake
* Visual Studio 2017 / 2019 / 2022
* NSIS (optional - needed to create install package)

**Note:**  The following instructions assume you are using the `git bash` terminal.  If you are using the `cmd` terminal, replace `rm` with `del` and `mkdir` with `md`.

### Clone the repository
```sh
git clone https://github.com/hpcc-systems/eclide.git eclide
cd eclide
git submodule update --init --recursive
```

### Remove old vcpkg.exe (if it exists)
```sh
rm ./vcpkg/vcpkg.exe
```

### Create a build folder (eclide/build)
```sh
mkdir build
cd build
```

### Generate Visual Studio Solution
```sh
cmake .. -A Win32
...or...
cmake .. -G "Visual Studio 16 2019" -A Win32
...or...
cmake .. -G "Visual Studio 17 2022" -A Win32
```

### Build
```sh
cmake --build . --config RelWithDebInfo --parallel
```

Supported Builds
```sh
cmake --build . --config Debug --parallel
cmake --build . --config RelWithDebInfo --parallel
cmake --build . --config Release --parallel
cmake --build . --config MinSizeRel --parallel
```

### Create Installer
```sh
cmake --build . --config RelWithDebInfo --target package --parallel
```
