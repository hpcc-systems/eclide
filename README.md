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

### Digitally signed installer

Code signing requires a sibling `sign` folder containing the certificate:
- `../sign/hpcc_code_signing.pfx` - The code signing certificate
- `../sign/passphrase.txt` - The certificate passphrase (optional if provided via CMake)

The installer will be automatically signed during the package build if the passphrase is available.

Alternatively, you can pass the signing passphrase at configure time:
```sh
cmake .. -A Win32 -DSIGNING_CERTIFICATE_PASSPHRASE="your_passphrase"
```

If the passphrase is not provided via the command line, CMake will attempt to read it from `../sign/passphrase.txt`.

To sign the package separately after building:
```sh
cmake --build . --config RelWithDebInfo --target SIGN --parallel
```
