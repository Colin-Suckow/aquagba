# AquaGBA

AquaGBA is 50% an attempt at learning (old) ARM and 50% at learning basic cmake. This emulator doesn't really do anything right now but maybe it will in the future.

## Clone and Build Instructions

Clone project with submodules and cd into it
```
$ git clone --recurse-submodules https://github.com/Colin-Suckow/aquagba
$ cd aquagba
```

Make build directory and cd into it
```
$ mkdir build
$ cd build
```

Generate build files
```
$ cmake ..
```

Make desktop client, emulator core, and unit tests
```
$ make
```

Run desktop client
```
$ ./aquagba-desktop/aquagba-desktop 
```

Run unit tests
```
$ ./aquagba-core-tests/aquagba-core-tests
```


