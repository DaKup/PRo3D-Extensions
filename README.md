# PRo3D-Extensions

https://github.com/vrvis/PRo3D

## Download Pre-Build Binaries

https://github.com/DaKup/PRo3D-Extensions/releases

## Build-Instructions

```bash
git clone https://github.com/DaKup/PRo3D-Extensions.git
```

- Download [SPICE Library](https://naif.jpl.nasa.gov/naif/toolkit_C.html)
- Extract zip inside source directory
- open cmd inside source directory

```bash
cmake --preset windows-configure
```

```bash
cmake --build --preset windows-build
```

[Get SPICE Kernels from here](https://s2e2.cosmos.esa.int/bitbucket/projects/SPICE_KERNELS)

e.g.
```bash
git clone --depth=1 https://s2e2.cosmos.esa.int/bitbucket/scm/spice_kernels/exomars2016.git
```

## Acknowledgements

NASA's Navigation and Ancillary Information Facility (NAIF) for their [SPICE Toolkit](https://naif.jpl.nasa.gov/naif/toolkit.html)
