# PRo3D-Extensions
https://github.com/vrvis/PRo3D


## Setup

```bash
git clone https://github.com/DaKup/PRo3D-Extensions.git
```

- Download [Spice Library](https://naif.jpl.nasa.gov/naif/toolkit_C.html)
- Extract zip inside source directory
- open cmd inside source directory

```bash
cmake --preset windows-configure
cmake --build --preset windows-build
```

[Get SPICE Kernels from here](https://s2e2.cosmos.esa.int/bitbucket/projects/SPICE_KERNELS)

e.g.
```bash
git clone --depth=1 https://s2e2.cosmos.esa.int/bitbucket/scm/spice_kernels/exomars2016.git
```
