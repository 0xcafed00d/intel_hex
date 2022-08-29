# Raspberry Pi C++ project template

Template C++ project for the raspberry pi pico, configured for CMake and Visual Studio Code.
Requires ```PICO_SDK_PATH``` evironment variable to be setup with the location of the Pico SDK.
Only tested on Linux.

## To Build:

``` 
mkdir build
cd build
cmake ..
make
cp template.uf2 /media/<user>/RPI-RP2
```

