# ECL IDE

##  Building on windows

### Prerequisites
* git (optional - needed to clone sources)
* cmake 3.23 or later
* Visual Studio 2019 or 2022 (with C++ workload)
* NSIS (optional - needed to create install package)

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

### Configure

CMake presets are provided for both Win32 (x86) and x64 targets using Visual Studio 2019 or 2022.

#### Visual Studio 2022

```sh
# Win32
cmake --preset vcpkg-VS-17

# x64
cmake --preset vcpkg-VS-17-x64
```

#### Visual Studio 2019

```sh
# Win32
cmake --preset vcpkg-VS-16

# x64
cmake --preset vcpkg-VS-16-x64
```

### Build

```sh
# Win32 RelWithDebInfo
cmake --build --preset VS-17-RelWithDebInfo

# Win32 Debug
cmake --build --preset VS-17-Debug

# x64 RelWithDebInfo
cmake --build --preset VS-17-x64-RelWithDebInfo

# x64 Debug
cmake --build --preset VS-17-x64-Debug
```

To build only the `ECLIDE` target, add `--target ECLIDE`:
```sh
cmake --build --preset VS-17-x64-RelWithDebInfo --target ECLIDE
```

### Create Installer
```sh
cmake --build --preset VS-17-x64-RelWithDebInfo --target package
```

### Digitally signed installer

Code signing requires a sibling `sign` folder containing the certificate:
- `../sign/hpcc_code_signing.pfx` - The code signing certificate
- `../sign/passphrase.txt` - The certificate passphrase (optional if provided via CMake)

The installer will be automatically signed during the package build if the passphrase is available.

Alternatively, you can pass the signing passphrase at configure time:
```sh
cmake --preset vcpkg-VS-17-x64 -DSIGNING_CERTIFICATE_PASSPHRASE="your_passphrase"
```

If the passphrase is not provided via the command line, CMake will attempt to read it from `../sign/passphrase.txt`.

To sign the package separately after building:
```sh
cmake --build --preset VS-17-x64-RelWithDebInfo --target SIGN
```
