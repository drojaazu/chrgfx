# chrgfx
A library for converting tile (aka CHR) bitmap graphics used in retro video games to and from PNG, with support for a wide range of hardware via external graphics definitions.

The library itself, libchrgfx, can be integrated into any C++ application. There are also two support utilities included, `chr2png` and `png2chr`. The former will convert raw tile and/or palette data to a PNG image, while the latter will take a sufficiently compatible PNG and return raw tile/palette data.

## Building
chrgfx requires [png++](https://www.nongnu.org/pngpp/) to be installed and uses CMake for the build process. From the root of the project directory:

    mkdir build && cd build
    cmake ..
    make
    sudo make install

## chr2png - Usage
`--gfx-def`,`-G`

Path to gfxdef file; if not specified, defaults to `/etc/chrgfx/gfxdefs`

`--profile`,`-P`

Specify which graphics profile to use

`--chr-def`,`-T` (required, see note)

Specify which tile definition to use; if `--chr-def` and `--profile` are both specified, `--chr-def` overrides the tile definition in the profile; only required if not using `--profile`, which should already have a tile definition

`--col-def`,`-C` (required, see note)

Specify which color definition to use; if `--col-def` and `--profile` are both specified, `--col-def` overrides the color definition in the profile; only required if `--pal-data` is supplied

`--pal-def`,`-L` (required, see note)

Specify which palette definition to use; if `--pal-def` and `--profile` are both specified, `--pal-def` overrides the palette definition in the profile; only required if `--pal-data` is supplied

`--chr-data`,`-c` (required)

Filepath of the raw tile (CHR) data

`--pal-data`,`-p`

Filepath of the raw palette data; if not specified, an 8-bit (256 color) palette of random colors will be generated

`--output`,`-o`  (required, see note)

Specify output PNG filepath; if not specifed, will output to stdout

`--columns`,`-d`

Specify the number of tile columns to render in the output image

`--subpalette`,`-s`

Specify the subpalette index to use in the output image; value is zero indexed (i.e. the first palette ie 0, second palette is 1, and so on); if not specified, the full palette will be used

`--trns`,`-t`

Enable transparency in output PNG image

`--trans-index`,`-i`

Specify palette index to use for transparency; if not specified, index 0 will be used

`--help`,`-h`

Display built in help

### Example
    chr2png --profile sega_md --chr-data sonic1_sprite.chr --pal-data sonic1.cram --trns --columns 32 > sonic1_sprite.png

## png2chr - Usage
`--gfx-def`,`-G`

Path to gfxdef file; if not specified, defaults to `/usr/local/lib/chrgfx/gfxdefs`

`--profile`,`-P` (required)

Specify which graphics profile to use

`--chr-def`,`-T` (required, see note)

Specify which tile definition to use; if `--chr-def` and `--profile` are both specified, `--chr-def` overrides the tile definition in the profile; only required if not using `--profile`, which should already have a tile definition

`--col-def`,`-C` (required, see note)

Specify which color definition to use; if `--col-def` and `--profile` are both specified, `--col-def` overrides the color definition in the profile; only required if `--pal-data` is supplied

`--pal-def`,`-L` (required, see note)

Specify which palette definition to use; if `--pal-def` and `--profile` are both specified, `--pal-def` overrides the palette definition in the profile; only required if `--pal-data` is supplied

`--png-data`,`-b` (required, see note)

Specify the input PNG image; if not specified, expects PNG data from stdin

`--chr-output`,`-c`

Specify filepath of output tile data; if not specified, `--pal-output` must be set

`--pal-output`,`-p` 

Specify filepath of output palette data; if not specified, `--chr-output` must be set

`--subpalette`,`-s`

Specify the subpalette index to use in the output image; value is zero indexed (i.e. the first palette ie 0, second palette is 1, and so on); if not specified, the full palette will be used

`--help`,`-h`

Display built in help

### Example
    cat crono_sprite.png | png2chr --profile nintendo_sfc --chr-output crono.chr --pal-output crono.pal --subpalette 0

## Graphics Definitions (gfxdef file)
See the readme in the gfxdef directory.

## Using libchrgfx
The core of chrgfx is libchrgfx, which contains all the subroutines and data structures for converting between formats and importing/exporting PNG images. Most of the documentation appears in the source code.

Headers are installed to `/usr/include/chrgfx` and the .so to `/usr/lib`. You can include `chrgfx.hpp` for all everything, or pick and choose headers for specific functions.

Please see the readme in the gfxdefs directory for a high level overview of some of the concepts and data structures.

## Non-gfxdef Formats / Custom Conversion Functions
Though the vanilla tile/color/palette definition system should work with most hardware, some formats may not fit cleanly into the algorithms. For example, the Super Famicom has a 3bpp quasi-format which won't work as a tile definition. Or if we want to import a modern format, such as palette data from TileLayer Pro or Paint Shop Pro, the basic palette converter function not sufficient.

In such cases, custom conversion code must be written and compiled into the program to be called referenced by gfxdef files with the `converter_to` and `converter_from` options. This is an advanced option that requires knowledge of C++. Please check the `conv_*` source code files for more details on implementing your own code. And pull requests to merge in your new format are certainly welcome!

# Console graphics primer
The below is provided for those who are new to tile based graphics or who want clarification on some of the concepts around which chrgfx is designed.

## Tile (aka CHR)
There are a number of ways that video games end up being displayed. There are vectors, in which elements are drawn based on on the math of their geometry. There are polygons, the basis for 3D gameplay that is now commonplace. There are LCD screens like the Game & Watch and Tiger handhelds, which have predrawn art that is displayed or cleared by electrical signals. And there are tile based games, most commonly used in the 2D arcade and home console hardware from the late 70's through the 90's. It is on the data from these tile based games that chrgfx is intended to operate.

The basic unit of graphics in these games is the tile. Tiles are essentially very small bitmaps, where a bitmap is a 2D plane of dots (pixels) with each dot having one color. These tiles combine much like a jigsaw puzzle to form the larger backgrounds and sprites and, ultimately, full image on the screen. Tiles are generally 8 by 8 pixels in size, though can have any dimension, and in later systems they were 16x16 or larger.

An old term for a graphics tile is "CHR," which is short for "char," which is short for "character." This evolved from the world of programming, where the char data type is an unsigned 8-bit (1 byte) value. It was commonly used for holding text characters. In early game development days, it became synonymous with the tile data, since the limited size of graphics storage meant that tiles coule be reference with just one byte, a char type. It is commonly associated with the Nintendo Famicom's CHR graphics data bank, though other contemporaneous hardware used similar terminology.

CHR is not really used as a term anymore, though I chose it for the program name to match the retro dev aesthetic. :)

## Colors
In the digital grapics sense, a color is composed of levels of red, green and blue which our eyes perceive as a certain hue. The amount of data allocated to each of these levels is measured in bits; this is known as color depth. Modern photographic images are 24 bit, with 8 bits (one byte) each for R, G and B levels. That gives us more than 16 million distinct colors to work with.

Older game hardware did not have such a large gamut available. The number of colors that could be generated was dependent on the video hardware in each system. For example, the Sega Mega Drive has 9 bit colors, which can generate up to 512 unique colors. Super Famicom colors are 15 bit (32,769 colors). And the original Gameboy is 2 bit, with a maximum of 4 colors.

(To be clear, the bit depth of colors has nothing to do with the video game advertising of the 1990's, which heralded the Super Famicom/Sega Mega Drive/etc as "16 bit," and the next generations as "32 bit" and "64 bit." That advertising was intentionally ambiguous ("bigger number = better!"), but it actually refers to the width of the data bus, which allowed more data to be addressable and thus more actual game content present. It is not related to graphics, per se.)

Some early consoles generated their colors based on television broadcast signal standards rather than levels of RGB, such as the Atari 2600 and the original Nintendo Famicom. In order to represent these, we have to create "best guess" equivalent RGB colors based on the output signal. Such systems are represented by a hardcoded palette rather than generated colors.

## Palettes
There are two main methods for storing the colors of a bitmap: direct and indexed. In direct color mode, *each pixel specifies its own red, green and blue levels*. This is used almost exclusively in our modern digital world, in photography and video game output, where data size is not so much of a concern.

In indexed color mode, there is a predefined list of RGB colors seperate from the pixels, and *each pixel specifies an entry from that color list*. This color list is known as a color palette. Indexed color images are smaller in data size than direct color since each pixel only needs enough bits to reference one of those palette entries. The tradeoff here is that the color list is small, usually 256 color maximum. This means indexed mode is no good for things like photographs, which need a full range of color to reproduce what we see. But it works great for imagery with a low number of colors, like cartoon artwork and video game pixel graphics.

It's important to understand the distinct between colors *possible* and colors *available.* As we said in the previous section, the Super Famicom has 15bit color, meaning 32,769 colors are *possible.* However, its color palette only has 256 entries, which means only 256 colors are *available* to be displayed at any one time.

Recall that the individual pixels of our indexed-mode tiles reference an entry number within a palette. Therefore, *the number of data bits allocated to a pixel determines the number of colors it can reference*. So, if a tile is limited to 2 bits of data per pixel, it can only have four values (00, 01, 10, 11), and can reference only four colors within the system palette. This introduces the concept of subpalettes: a subpalette is a sequential array of colors within the system palette. Mega Drive tile graphics are 4bpp (four bits per pixel), which means each pixel can access 16 colors from within the system palette. So, the system palette of 64 colors is neatly broken down into four subpalettes of 16 colors each. Tile graphics can then use any of those four palettes.

## Graphics As Data
So now that we have explained things at a theoretical level, let's look at how these concepts are implemented in data. It's important to have a basic understanding of how the data is stored and arranged when attempting to create graphics definitions.

### Tile Data
Since we already know the dimensions of tile and since the color/palette data is stored elsewhere, tile data is simply an array of pixels, nothing more.

The data layout of those pixels is organized in one of two ways: packed or planar. [Packed pixel](https://en.wikipedia.org/wiki/Packed_pixel) (also called linear or "chunky") format is the more intuitive of the two. In this style, *the sequential data represent one whole pixel.* Let's say we're working with 4bpp (bits per pixel) graphics. In a packed format, every *sequential* four bits represent a pixel. So the first byte of data is the first two pixels, the next byte is the next two pixels, and so on.

The concept of [planar graphics](https://en.wikipedia.org/wiki/Planar_(computer_graphics)) takes some knowledge of binary to fully understand. First, know that a pixel value is the sum of each set data bit. With that in mind, if we break out each of those bits and visualize them as "layers" that combine to form a value, we arrive at the concept of [bitplanes](https://en.wikipedia.org/wiki/Bit_plane). *A bitplane is the group of all digits at a given bit position.* Bitplane 0 is the group of bit 0 of all data in a tile; bitplane 1 is the group of bit 1 and so one. The sum of the digits at a certain pixel offset within each bitplane generates our final pixel color.

Let's break down some data. If we're working with 4bpp tiles, then each pixel has four bits. In an 8x8 tile, that's 64 pixels * 4bpp = 256 bits. That means there are 64 bits of data in bit 0 across all pixels; and 64 bits of data for bit 1, and for bit 2 and bit 3.

In packed format, bits 0 to 3 for each pixel are "packed" together, with each pixel stored sequentially.

    pixel number  pix0 pix1 pix2 pix3 pix4 ...
       pixel bit  3210 3210 3210 3210 3210 ...

In planar format, the values of bit 0 for all pixels are stored together, followed by the values of bit 1 for all pixels, and so on.

    pixel number pix0 pix1 pix2 ... pix0 pix1 pix2 ... pix0 pix1 pix2 ...
       pixel bit  3    3    3   ...  2    2    2   ...  1    1    1   ...

*In packed format, data is grouped by pixels; in planar format, data is grouped by bitplanes.*

The [Neo-Geo Development Wiki has an excellent visualization](https://wiki.neogeodev.org/index.php?title=Sprite_graphics_format) of planar data that may help if you're having trouble understanding the concept.

Ultimately, the different formats used on different hardware are variations on bitdepth and the arrangement of bitplanes/pixels. A tile definition used by chrgfx is a "map" of this arrangement, used to decode the tile graphics.

### Color Data
RGB colors are stored in as many bytes as it takes to contain all the data bits for each component; this is between 1 and 3 bytes.

Since color data is operated on as a single data structure (8-bit byte, 16-bit short, 32-bit long), it is important to take into consideration the [endianness](https://en.wikipedia.org/wiki/Endianness) of the hardware you are working with because it will affect the order of the stored data.

For example, Super Famicom colors are 5 bit, so for all three color components, we have 15 bits. This takes up two bytes (16bits, with one of the bits unused). It looks something like this:

    15|       |8  7|       |0
      xBBBBBGG      GGGRRRRR

However, the SNES is a little endian system, which means the least significant bytes of a number are stored first. So in data storage, the data appears like this:

    7|       |0  15|       |8
      GGGRRRRR      xBBBBBGG

(It's important to note that endianness does not matter in memory. Data is always stored with most significant bits to the "left" and less significant bits to the "right." Endianness only matters in stored data, with which we are obviously working.)

It's important to specify if the color data is little or big endian so it can be properly converted.

Note that a color data is only for devices which have colors with RGB components. Hardware that uses YIQ or other such non-RGB colorspaces will need a pre-defined palette of color approximations.

### Palette Data
A palette is simply an array of all the color values, ordered seqentially. So a palette of 64 entries of 16 bit colors would be 128 bytes in size, with color 0 at offset 0, color 1 at offset 2, and so on. There are some quirks on some systems (such as the Virtual Boy, which only uses 6 bits for a palette yet is stored in 2 bytes...), but they are rare. Palette data is just an array of the color values we previously discussed.

## Special Thanks
Special thanks to:

- Klarth, for his venerable `consolegfx.txt`, which was my introduction to the data side of tiled graphics and was instrumental in my early development work on Dumpster and now chrgfx.

- The MAME Team, for their work in documenting hardware through source code and for inspiring some solutions that are essential to chrgfx.
