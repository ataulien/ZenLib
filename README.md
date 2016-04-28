# ZenLib
Loading of proprietary formats used by the engine used by the games "Gothic" and "Gothic II".

### Features
Contains loaders for:
 - Zen-Archives (ASCII, BinSafe)
 - VDF-Archives
 - Compiled-Mesh formats (Static, Skeletal)
 - Skeleton-Hierarchy
 - Animation-Samples

There is no possibility to write files at the moment. However, this is planned for a later release.

# Building
### Linux
```sh
$ mkdir build
$ cd build
$ cmake ..
$ make
```
### Windows
Use CMake-GUI to generate project-files for your favorite build-system/IDE. Then proceed to build the library as usual.

# Samples
There are some sample programs inside the */samples*-folder, which can teach you how the library works and what you can do with it.

# License
MIT, see License-file.
