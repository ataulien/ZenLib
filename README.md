# ZenLib
Loading of proprietary formats used by the engine of the games "Gothic" and "Gothic II".

> Currently, only Gothic II is supported

### Features
Contains loaders for:
 - Zen-Archives (ASCII, BinSafe)
 - VDF-Archives
 - Compiled-Mesh formats (Static, Skeletal)
 - Skeleton-Hierarchy
 - Animation-Samples
 - Compiled textures

There is no possibility to write files at the moment. However, this is planned for a later release.

# Building
> ZenLib requires a compiler capable of the C++11-standard and at least CMake 3.1!

### Linux
```sh
$ cd <project-root>
$ mkdir build
$ cd build
$ cmake ..
$ make
```
### Windows
Use CMake-GUI to generate project-files for your favorite build-system/IDE. Then proceed to build the library as usual.

# Samples
There are some sample programs inside the */samples*-folder, which can teach you how the library works and what you can do with it.

# Basic usage
### VDF-Archives
```cpp
#include <vdfs/fileIndex.h>

/** ... **/

// Load all vdfs you need into a file-index
VDFS::FileIndex vdf;
vdf.loadVDF("Meshes.vdf");
vdf.loadVDF("MyMod.mod");

// Get file-data as byte-vector. Filename only, no folders.
std::vector<uint8_t> data;
vdf.getFileData("MyAsset.ext", data);
```

### ZEN-Archives
```cpp
#include <zenload/zenParser.h>

/** ... **/

 // Load ZEN from disk. There is also a constructor for byte-data, usefull if loading from a .vdf.
 ZenLoad::ZenParser parser("MyWorld.zen");
 
 // Do parsing
 parser.readHeader();
 ZenLoad::oCWorldData world = parser.readWorld();
 ZenLoad::zCMesh* mesh = parser.getWorldMesh();
 
 // Do something with 'world' or 'worldMesh'
```

### Meshes/Animations
```cpp
 // Load by filename + initialized VDFS::FileIndex
 ZenLoad::zCProgMeshProto mesh("MyMesh.MRM", vdfIndex);

 // Bring the loaded mesh in a more accessible format
 ZenLoad::PackedMesh packedMesh;
 mesh.packMesh(packedMesh);
```
> This is mostly the same for all "zC***"-Classes in the ZenLib-Package.

> See *zenload/zTypes.h* for more information about the packed data structs returned by the objects.

### Textures
```cpp
#include <zenload/ztex2dds.h>

/** ... **/
std::vector<uint8_t> zTexData = ...; // Get data from vdfs or something

// Convert the ZTex to a usual DDS-Texture
std::vector<uint8_t> ddsData;
ZenLoad::convertZTEX2DDS(zTexData, ddsData);

// ... do something with ddsData
// or...

// Convert the DDS-Texture to 32bpp RGBA-Data, if wanted
std::vector<uint8_t> rgbaData;
ZenLoad::convertDDSToRGBA8(ddsData, rgbaData);

// .. do something with rgbaData
```

### Log-Callback
By default, the internal Logging-Class will output to stdout (and OutputDebugString on Windows).

You can define your own target by calling:
```cpp
#include <utils/logger.h>

/** ... **/

Utils::Log::SetLogCallback([](const std::string& msg){
       // Do something with msg
   });
```

> (Logging to file seems currently broken, sorry.)

# License
MIT, see License-file.
