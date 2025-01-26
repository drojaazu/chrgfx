# chrgfx
A library with CLI utilities for converting tile (aka CHR) based graphics used in retro video games to and from bitmap images, with support for a wide range of hardware via external graphics definitions. It is useful for viewing data as well as converting modern graphics into native formats for retro game development.

## Building

chrgfx requires libpng and the [png++](https://www.nongnu.org/pngpp/) wrapper for the support utilities. These are available in most distro repositories. Installing the wrapper will include libpng as a dependency. In Arch Linux, the wrapper package is `png++`; in Debian, it is `libpng++-dev`.

If you wish to build only libchrgfx without the utilities (and thus without the need for the png packages), pass `-DNO_UTILS=1` when running cmake.

CMake is used for the build process. From the root of the project directory:

    mkdir build && cd build
    cmake ..
    make
    sudo make install

# Utilities
There are three support utilities included: `chr2png`, `png2chr`, and `palview`.

## chr2png

This will convert encoded tile/palette data to a PNG image.
 
## png2chr

This will take a sufficiently compatible PNG and output encoded tile/palette data.

Compatible in this case means a PNG in indexed color mode with a 256 color palette.

## palview

This will take a color palette and generate an image of color swatches corresponding to the palette.

## Graphics Definitions (gfxdefs)

The conversion routines rely on graphics definitions (gfxdef), which describe the format of the data for encoding and decoding. There are three kinds of definitions: tile (chrdef), palette (paldef) and color (coldef).

Graphics definitions can be mixed and modified at run time. With this system, chrgfx is extensible and can support practically any tile-based hardware.

Graphics definitions are loaded from three possible sources.

### Internal Definitions

The chrgfx library has a number of common, generic definitions included. Please see the [builtin_defs.hpp source file](src/chrgfx/builtin_defs.hpp) for a list.

### gfxdefs File

External graphics definitions are stored in the `gfxdefs` file. The project comes with a number of definitions for many common hardware systems already created in this file.

Please [see the readme in the gfxdef directory](share/gfxdef/README.md) and [the gfxdefs file itself](share/gfxdef/gfxdefs) for more details on the format.

### CLI Definitions

Graphics definitions can also be defined on the command line. This is useful for testing or dealing with dynamic formats (for example, perhaps tile sizes are not consistent across multiple files when running a batch conversion).

Definitions specified on the command line override definitions loaded from internal/external sources in a piecemeal fashion. For example, you may load the `col_bgr_222_packed` coldef and then specify `--col-big-endian` on the command line to change the endianness aspect only.

### Hardware Profiles

For convenience, there are also profiles, which are groupings of a single chrdef, paldef and coldef to represent the graphics subsystem of a certain piece of hardware. In this way, it is simple for the user to specify which decoders/encoders to use for certain hardware. Moreover, we reduce redundancies for hardware that may share one type of definition, but not others.

For example, the Sega Master System and Game Gear have the same tile format, but the palette and color encodings are different. We can make a different profile for each system, with both using the same tile encoding but seperate palette and color encodings. Another example is the original Gameboy and the later Gameboy Pocket. They are exactly the same in hardware, but the original has a more green tint while the Pocket is more gray. We can create profiles for both, with the same tile and palette formats, but with a different color list to simulate their different perceived colors.

The entries within the profile can be override by specifying a chrdef, paldef or coldef ID seperately on the command line.

### gfxdef Loading Order

Because definitions can be loaded from multiple sources, there is an ordering for processing to determine what definition is finally used:

1. If a profile is specified, the external gfxdefs file is checked first to get the target chrdef/paldef/coldef from that profile.
2. The internal list is checked for the target gfxdef.
3. If not present in the internal list, the gfxdef file is checked for the target gfxdef. If a chrdef/paldef/coldef ID was specified and was not found at this point, an error occurs.
4. If a tile or color definition option is specified on the command line, that modifes the definition

NOTE: If a chrdef/paldef/coldef or profile ID was NOT specified, steps 1 to 3 are skipped and it is assumed all required gfxdefs are fully defined on the command line.

## Usage

### Shared Options

The following options are available for all three utilitiese (`chr2png`, `png2chr`, `palview`)

`--help`, `-h`

Display built in program usage

`--gfx-def <filepath>`, `-G <filepath>`

Path to gfxdef file. If not specified, it checks for:

 - `${XDG_DATA_HOME}/chrgfx/gfxdefs`
 - `${XDG_DATA_DIRS}/chrgfx/gfxdefs`

`--profile <hardware_profile_id>`, `-H <hardware_profile_id>`

Specify hardware profile to use

`--chr-def <tile_encoding_id>`, `-T <tile_encoding_id>`

`--col-def <color_encoding_id>`, `-C <color_encoding_id>`

`--pal-def <palette_encoding_id>`, `-P <palette_encoding_id>`

These arguments specify the tile, color and palette encoding, respectively. They are only required if a graphics profile was not specified. If they are used in conjunction with a graphics profile, they will override that particular encoding. (For example, using `--chr-def` will override the tile encoding that was specified in the profile.)

Please [see the readme in the gfxdef directory](share/gfxdef/README.md) for more information about values represent in a gfxdef.

The following options are used to build or modify a gfxdef:

`--chr-width <integer>`

Specify the width of a tile

`--chr-height <integer>`

Specify the height of a tile

`--chr-bpp <integer>`

Specify the bits per pixel (BPP) of a tile

`--chr-plane-offsets <p0,p1,p2...|[start:count:step]>`

Specify the offset (in bits) to the start of each plane within a pixel.

This is written as either a comma-delimted list of values, or as a range in the format of [start:count:step]. Please [see the readme in the gfxdef directory](share/gfxdef/README.md) for more about this option and list/range formatting.

The number of entries should match the bits per pixel value for the tile.

`--chr-pixel-offsets <p0,p1,p2...|[start:count:step]>`

Specify the start (in bits) to the start of each pixel within a row.

Value is specified in the same format as `--chr-plane-offsets`.

The number of entries should match the width of the tile.

`--chr-row-offsets <r0,r1,r2...|[start:count:step]>`

Specify the offset (in bits) to the start of each pixel row within a tile.

Value is specified in the same format as `--chr-plane-offsets`.

The number of entries should match the height of the tile.

`--pal-datasize <integer>`

Specify the size (in bits) of one palette.

This is optional when defining a palette and should rarely be needed. It is only necessary when the size of palette differs entry datasize * palette length. For example, if a color entry is 2 bits with a palette length of 4, but the palette is stored in a 16 bit value. In such a case, the `--pal-datasize` should be 16.

`--pal-entry-datasize <integer>`

Specify the size (in bits) of a color entry.

`--pal-length <integer>`

Specify the number of entries in a single palette.

`--col-bitdepth <integer>`

Specify the bitdepth of a color.

`--col-layout <red_offset,red_size,green_offset,green_size,blue_offset,blue_size>`

Specify the groupings of red, green and blue color channels within a color value. This should be a comma delimited list of exactly 6 entries, in this format: red_offset, red_size, green_offset, green_size, blue_offset, blue_size

Note that, for simplicity's sake, only one layout pass can be done via the command line. If you need to work with a complex color format that requires multiple layout passes, please use an external file (gfxdefs).

`--col-big-endian <1|0|true|false>`

Specify that the color data is big-endian. Data is processed as little-endian by default.

### chr2png - Additional Options

`--chr-data <filepath>`, `-c <filepath>`

Required; path to the encoded tile data

`--pal-data <filepath>`, `-p <filepath>`

Path to the encoded palette data; if not specified, a palette of random colors will be generated

`--output <filepath>`, `-o <filepath>`

Path to output PNG image; if not specifed, will output to stdout

`--pal-line <integer>`, `-l <integer>`

Specify the palette line (also called the subpalette) to use for rendering when passing in palette data that contains more tha one palette

`--row-size <integer>`, `-r <integer>`

Specify the number of tiles in a row in the output PNG

`--trns-index <integer>`, `-i <integer>`

Specify a palette index to use for transparency. This is often index 0. If not specified, the output image will not have transparency.

### Example Usage
    chr2png --profile sega_md --chr-data sonic1_sprite.chr --pal-data sonic1.cram --trns --row-size 32 > sonic1_sprite.png

## png2chr - Additional Options

`--png-data <filepath>`, `-b <filepath>` 

Path to the input PNG image; if not specified, expects PNG data piped from stdin

`--chr-output <filepath>`, `-c <filepath>`

`--pal-output <filepath>`, `-p <filepath>`

Path to the output tile and palette data, respectively. One or both must be specified.

### Example
    png2chr --profile nintendo_sfc --chr-output crono.chr --pal-output crono.pal < crono_sprite.png

### palview - Additional Options

Images are generated with indexed color, unless `--full-pal` is specified, in which case direct color is used.

`--pal-data <filepath>`, `-p <filepath>`

Path to the encoded palette data; if not specified, a palette of random colors will be generated

`--pal-line <integer>`, `-l <integer>`

Specify the palette line (also called the subpalette) to use for rendering when passing in palette data that contains more tha one palette.

Ignored when `--full-pal` is specified.

`--full-pal`, `-f`

Renders an image with multiple palettes. It will draw as many palettes as the data provided contains.

`--output <filepath>`, `-o <filepath>`

Path to output PNG image; if not specifed, will output to stdout

### Example Usage

## Special Thanks

Special thanks to:

- Klarth, for his venerable `consolegfx.txt`, which was my introduction to the data side of tiled graphics and was instrumental in my early development work on Dumpster and now chrgfx.

- The MAME Team, for their work in documenting hardware through source code and for inspiring some solutions that are essential to chrgfx.

- UCC BLACK canned coffee.

- Greetz to dosdemon, mdl, sebmal, lord, eri, freem and the rest of the internet graybeards from IRC and the old scene.
