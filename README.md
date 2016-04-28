# ZenLib
Loading of proprietary formats used by the engine used by the games "Gothic" and "Gothic II".

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
```
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
```
#include <zenload/zenParser.h>

/** ... **/

 // Load ZEN from disk. There is also a constructor for byte-data, usefull if loading from a .vdf.
 ZenLoad::ZenParser parser("MyWorld.zen");
 
 // Do parsing
 parser.readHeader();
 ZenLoad::oCWorldData world = parser.readWorld();
 
 // Do something with 'world'
```

### Meshes/Animations
```
 // Load by filename + initialized VDFS::FileIndex
 ZenLoad::zCProgMeshProto mesh("MyMesh.MRM", vdfIndex);

 // Bring the loaded mesh in a more accessible format
 ZenLoad::PackedMesh packedMesh;
 mesh.packMesh(packedMesh);
```
> This is mostly the same for all "zC***"-Classes in the ZenLib-Package.

> See *zenload/zTypes.h* for more information about the packed data structs returned by the objects.

### Log-Callback
By default, the internal Logging-Class will output to stdout (and OutputDebugString on Windows).

You can define your own target by calling:
```
#include <utils/logger.h>

/** ... **/

Utils::Log::SetLogCallback([](const std::string& msg){
       // Do something with msg
   });
```

> (Logging to file seems currently broken, sorry.)

# License
MIT, see License-file.
