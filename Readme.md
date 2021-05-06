# esp32base

Proyecto base con libreria barias para desarrollo en ESP32 sobre la plataforma Arduino

## Build
### Consola
Utilizar el script de cmake: [Arduino-Cmake-Toolchain](https://github.com/a9183756-gh/Arduino-CMake-Toolchain)

Windows
```
mkdir cmbuild
cd cmbuild
cmake -D CMAKE_TOOLCHAIN_FILE=..\..\tools\Arduino-CMake-Toolchain\Arduino-toolchain.cmake ..\ -G "MinGW Makefiles"
#Elegir la placa adecuada en el archivo generado BoardOptions.cmake
cmake ..\ -G "MinGW Makefiles"
mingw32-make
```

Linux
```
mkdir cmbuild
cd cmbuild
cmake -D CMAKE_TOOLCHAIN_FILE=..\..\tools\Arduino-CMake-Toolchain\Arduino-toolchain.cmake ..\ -G "Unix Makefiles"
make
```

### VSCode
Utilizar el plugin "Arduino" y renombrar el "sketch" principal a appbase.ino

### Arduino IDE
Renombrar el "sketch" principal a appbase.ino

## Upload
```
mingw32-make upload-esp32base SERIAL_PORT=COM4
```
