# chrgfx
Converts tile (CHR) based bitmap graphics from retro hardware systems, similar to Tile Layer Pro and such, with the ability to support a large amount of hardware through user-defined graphics definitions.

## Usage
`--gfx-def`,`-G`

Path to gfxdef file (i.e. the graphics format) to use for tile and palette data.

`--chr-def`,`-C`

Path to gfxdef file or name of internal definition to use for tile data. Has precedence over `--gfx-def` option.

`--pal-def`,`-P`

Path to gfxdef file or name of internal definition to use for palette data. Has precedence over `--gfx-def` option.

`--chr-data`,`-c`

Path to tile data. If not specified, data will be read input from stdin.

`--pal-data`,`-p`

Path to palette data.

`--output`,`-o`

Path to output PNG file. If not specified, image will be written to stdout.

`--trns`,`-t`

Enable palette transparency. Default transparent index is 0.

`--trns-index`,`-i`

Specify palette index to use as transparency. (Zero indexed)

`--columns`,`-d`

Number of tile columns (i.e. tile width) to render for output image.

`--subpalette`,`-s`

Specify subpalette index. (Zero indexed)

### Usage Example
    cat ../etc/sonic_sprite | ./chrgfx --gfx-def gfxdef/sega_md.gfxdef --pal-data ../etc/sonic1.cram --trns --columns 32 > test.png

## Graphics Definitions (gfxdef files)
Rather than hardcoding conversion routines for each format, chrgfx uses generalized algorithms which support the great majority of CHR based graphics hardware. The algorithms rely on layout information to interpret data, and such information comes from user-defined *graphics definition* (gfxdef) files.

There are a number of pre-created gfxdef files included with the project, and you can extend chrgfx to support practically any system by creating your own definitions. You should have a solid understanding binary, bit-level data, and endianness when working to create your own definition files.

In some cases, a format may not be compatible with the generalized algorithms (such as Super Famicom 3bpp, or palette data from PC applications like TileLayer Pro). To accomodate these "non-standard" formats, a custom conversion function must be written. We'll discuss those further down.

### Definition File Format
A gfxdef is a plaintext file made up of key-value pairs, one per line. Keys are one word, while values are either a single or comma-delimited list of numbers. Comments can be added with a # prefix. For example:

    # Sega Mega Drive definitions
    # tile format
    width 8
    height 8
    bpp 4
    planeoffset 0,1,2,3
    xoffset 0,4,8,12,16,20,24,28

Settings for tiles and palettes/colors are all uniquely named, so a single gfxdef file can contain both types. Only one definition set per type is allowed, however, due the the simple nature of the definition file format. The types can be mixed and matched via the `--chr-def` and `--pal-def` options to use only those types from two definition files.

### Tile Definitions
Tile definitions are based on the GfxLayout structure from MAME. The format is described by specifying the bit-level offset of bitplanes and pixels on the x and y axes.

Jumping right in to a relatively simple example: Sega Mega Drive tiles are 8x8 4bpp packed pixel format. Since it is packed, the bit data for each pixel is stored sequentially; therefore, the offset for bitplanes 0 to 3 is 0,1,2,3. Since each pixel is four bits and the pixels are stored sequentially, the offset for each pixel moving horizontally (i.e. each pixel in a row) is 0,4,8,12,16,20,24,28. Since each row is 8 pixels * 4bpp, each row is 32 bits in size. Thus, the data offset for each row moving vertially is 0,32,64,96,128,160,192,224.

Note that the number of entries in the bitplane offset list must match the number of bitplanes. In the above example, there are four bitplanes (4bpp), so there are four entries in the bitplane offset list. Similarly, the number of entries in the X offset must match the pixel width, and the Y offset count must match the height.

Our Mega Drive tile definition ends up looking like this:

    width 8
    height 8
    bpp 4
    planeoffset 0,1,2,3
    xoffset 0,4,8,12,16,20,24,28
    yoffset 0,32,64,96,128,160,192,224

Nintendo Super Famicom tiles are a bit more complex. They are 8x8 4bpp planar format, with bitplanes 0 and 1 grouped together followed by bitplanes 2 and 3. Each byte (8 bits) represents a bitplane for the row. The first 16 bytes make up bitplanes 0 and 1 of each row, while the last 16 bytes make up bitplanes 2 and 3. This layout from Klarth's consolegfx document helps visualize the data:

    [r0, bp0], [r0, bp1], [r1, bp0], [r1, bp1], [r2, bp0], [r2, bp1], [r3, bp0], [r3, bp1]
    [r4, bp0], [r4, bp1], [r5, bp0], [r5, bp1], [r6, bp0], [r6, bp1], [r7, bp0], [r7, bp1]
    [r0, bp2], [r0, bp3], [r1, bp2], [r1, bp3], [r2, bp2], [r2, bp3], [r3, bp2], [r3, bp3]
    [r4, bp2], [r4, bp3], [r5, bp2], [r5, bp3], [r6, bp2], [r6, bp3], [r7, bp2], [r7, bp3]

As a gfxdef, it looks like this:

    width 8
    height 8
    bpp 4
    planeoffset 136,128,8,0
    xoffset 0,1,2,3,4,5,6,7
    yoffset 0,16,32,48,64,80,96,112

### Tile Definition Settings Reference

`width`, `height` - The pixel dimensions of a single tile

`bpp` - The number of bits per pixel

`planeoffset` - The offset (in bits) of each bitplane. **The number of entries here must match the value of `bpp`.**

xoffset - The offset (in bits) of each pixel column (x axis). **The number of entries here must match the value of `width`.**

yoffset - The offset (in bits) of each pixel row (y axis). **The number of entries here must match the value of `height`.**

### Palette Definitions
Palette definitions encapsulate formats of both the indexed color list (i.e. the palette) and the colors themselves. They are meant to describe the data as it appears inside the original machine's video RAM, which is simply an array of color definitions. Therefore, we describe a palette by specifying the size of the system palette, the size of a subpalette and the number of subpalettes available. Since the entries in a palette are often multi-byte values, we also need to take into consideration whether the work data is little or big endian.

The colors are generated either by specifying RGB levels or by providing a preset palette of colors for systems that do not use RGB natively. For example, the original Nintendo Famicom uses the YIQ colorspace and encoded the output as a TV signal, so we do not have an exact representation of the color components. Instead, we provide a list of preset RGB colors, called a reference palette, that will be used to approximate the output:

    refpal #545454,#001E74,#08102C,...

Most hardware from the late 80's on used RGB colors, however. In this case, we specify the number of bits of data each color component (red, green, blue) has and where that bit level data is within the whole color value.

Color decoding works by shifting each color component into the LSB and ANDing out the color bits. For example, Super Nintendo 5 bit colors are laid out like this:

    15|               |0
      xBBBBBGG GGGRRRRR

So green needs to be shifted 5 bits, blue 10 bits and red 0 bits. We also specify the size of each component; in this case, all three are 5 bits in size. The color definition looks like this:

    red_shift 0
    red_size 5
    green_shift 5
    green_size 5
    blue_shift 10
    blue_size 5

This works fine for systems where the color components are sequential (which is most of them), but in cases where they are be split up, we'll need to make multiple passes. For example, Neo-Geo colors are more complicated:

    15 |                               |0
       DDR0G0B0R4R3R2R1 G4G3G2G1B4B3B2B1
    D = "dark bit", acts as LSB for all color components

(The "dark bit" here is a shared LSB for all color components; we're going to pretend it doesn't exist for this example so it's not any further complicated. Please refer to the included NeoGeo gfxdef file if you need a format reference.)

In this case, bit 0 of each component is in the upper byte. So on our first pass, we need to grab bit 0, then on the second pass grab the rest of the data. The color def ends up looking like this:

    color_passes 2
    red_shift 14,8
    red_size 5
    green_shift 13,4
    green_size 5
    blue_shift 12,0
    blue_size 5

To be clear, **you should not use a refpal AND a color definition.** It's one or the other. If for some reason both are specified, the refpal takes precedence.

### Palette Definition Settings Reference

`entry_datasize` - The size (in bits) of one entry in the color palette

`subpal_length` - The number of entries in a single subpalette

`subpal_count` - The number of subpalettes within the system palette

`subpal_datasize` - Manually specify the size of a subpalette (in bits). Used for cases where effective subpalette data is less than 8 bits, but is stored in 2 or more bytes. Optional; default is 0 (unused)

`big_endian` - Indicates the system is big endian rather than small endian. 1 is true, 0 is false. Optional; default is 0 (little endian)

`refpal` - An array of HTML formatted (e.g. #001E74) colors to simulate output of non-RGB hardware. The entries can be listed with or without the hash symbol (Either 001E74 or #001E74 is acceptable.)

`color_passes` - Specifies the number of times the shift/extra process will be performed on color data. Optional; default is 1. **Note that if this value is higher than 1, then EACH of below shift/size settings MUST have a number of entries equal to color_passes.**

`red_shift` - The number of bit places to shift the red component data into the LSB.

`green_shift` - The number of bit places to shift the green component data into the LSB.

`blue_shift` - The number of bit places to shift the blue component data into the LSB.

`red_size` - The number of bits of data making up the red component.

`green_size` - The number of bits of data making up the green component.

`blue_size` - The number of bits of data making up the blue component.

## Non-standard Formats
Though the tile/color/palette definitions should work with most systems, some formats may not fit cleanly into the algorithms. For example, the Super Famicom has a 3bpp quasi-format which won't work as a tile definition. Or if we want to import a modern file, such as palette data from TileLayer Pro or Paint Shop Pro, definition files are not sufficient.

In such cases, a solution will have to be hardcoded into the application. If you wish to add such formats, refer to the internal_defs, chr_conv and pal_conv source files for more information. And pull requests to merge in your new format are certainly welcome!

## Console graphics primer
The below is provided for those who are new to tile based graphics or who want clarification on some of the concepts around which chrgfx is designed.

### Tile (aka CHR)
Video games are a primarily visual medium, and there are a number of ways they can be displayed. There are vectors, in which elements are drawn based on their geometric properties. There are polygons, the basis for 3D gameplay that is now commonplace. There are LCD screens like the Game & Watch and Tiger handhelds, which have predrawn art that is displayed or cleared by electrical signals. And there are tile based games, popular in arcade and home console hardware from the late 70's through the 90's. It is on the data from these tile based games that chrgfx is intended to operate.

The basic unit of graphics in these games is the tile. Tiles are essentially very small bitmaps, where a bitmap is a 2D plane of dots (pixels) with each dot having one color. These tiles combine much like a jigsaw puzzle to form the larger backgrounds and sprites and, ultimately, full image on the screen. Tiles are generally 8 by 8 pixels in size, though can have any dimension, and in later systems they were 16x16 or larger.

An old term for a graphics tile is "CHR," which is short for "char," which is short for "character." This evolved from the world of programming, where the char data type is an unsigned 8-bit (1 byte) value. It was commonly used for holding text characters. In early game development days, it became synonymous with the tile data, since the limited size of graphics storage meant that tiles coule be reference with just one byte, a char type. It is commonly associated with the Nintendo Famicom's CHR graphics data bank, though other contemporaneous hardware used similar terminology.

CHR is not really used as a term anymore, though I chose it for the program name to match the retro dev aesthetic. :)

### Colors
In the digital grapics sense, a color is composed of levels of red, green and blue which our eyes perceive as a certain hue. The amount of data allocated to each of these levels is measured in bits; this is known as color depth. Modern photographic images are 24 bit, with 8 bits (one byte) each for R, G and B levels. That gives us more than 16 million distinct colors to work with.

Older game hardware did not have such a large gamut available. The number of colors that could be generated was dependent on the video hardware in each system. For example, the Sega Mega Drive has 9 bit colors, which can generate up to 512 unique colors. Super Famicom colors are 15 bit (32,769 colors). And the original Gameboy is 2 bit, with a maximum of 4 colors.

(To be clear, the bit depth of colors has nothing to do with the video game advertising of the 1990's, which heralded the Super Famicom/Sega Mega Drive/etc as "16 bit," and the next generations as "32 bit" and "64 bit." That advertising was intentionally ambiguous ("bigger number = better!"), but it actually refers to the width of the data bus, which allowed more data to be addressable and thus more actual game content present. It is not related to graphics, per se.)

Some early consoles generated their colors based on television broadcast signal standards rather than levels of RGB, such as the Atari 2600 and the original Nintendo Famicom. In order to represent these, we have to create "best guess" equivalent RGB colors based on the output signal. Such systems are represented by a hardcoded palette rather than generated colors.

### Palettes
What you see on the screen from a video game is ultimately a bitmap: a two-dimensional plane of dots, with each dot representing a single color.

There are two main methods for storing the colors of a bitmap: direct and indexed. In direct color mode, *each pixel specifies its own red, green and blue levels*. This is used almost exclusively in our modern digital world, in photography and video game output, where data size is not so much of a concern.

In indexed color mode, there is a predefined list of RGB colors seperate from the pixels, and *each pixel specifies an entry from that color list*. This color list is known as a color palette. Indexed color images are smaller in data size than direct color since each pixel only needs enough bits to reference one of those palette entries. The tradeoff here is that the color list is small, usually 256 color maximum. This means indexed mode is no good for things like photographs, which need a full range of color to reproduce what we see. But it works great for imagery with a low number of colors, like cartoon artwork and video game pixel graphics.

While the video hardware may be able to generate hundreds or thousands of colors, only a portion of these colors can be displayed on the screen at one time. Hardware like this has a system palette, which is modified by the game code as it runs and acts as the pool of colors available to the graphics on the screen. Using the Sega Mega Drive as an example again, the system palette has space for 64 entries, meaning there can be a maximum of 64 colors on screen at once across the sprites and background planes.

Recall that the individual pixels of our indexed mode tiles reference an entry number within a palette. Therefore, *the number of data bits allocated to a pixel determines the number of colors it can reference*. So, if a tile is limited to 2 bits of data per pixel, it can only have four values (00, 01, 10, 11), and can reference only four colors within the system palette. This introduces the concept of subpalettes: a subpalette is a sequential index of colors within the system palette. Mega Drive tile graphics are four bit, which means each pixel can access 16 colors from within the system palette. So, the system palette of 64 colors is neatly broken down into four subpalettes of 16 colors each. Tile graphics can then use any of those four palettes.

## Graphics Data
So now that we have explained things at a theoretical level, let's look at how these concepts actually appear in data. It's important to have a basic understanding of how the data is composed when working with format definitions, which are the key to decoding graphics with chrgfx.

### Tile Data
Since storage was at a premium during the "16-bit era" and reducing size was the name of the game, as it were, the data itself is straightforward. Since we already know the dimensions of the tile and since the palette data is stored elsewhere, a tile is just an array of pixels without any extra data.

The data for those pixels are organized in one of two ways: packed or planar. [Packed pixel](https://en.wikipedia.org/wiki/Packed_pixel) (also called linear or "chunky") format is the more intuitive of the two. In this style, the sequential data represent one whole pixel. Let's say we're working with 4bpp (bits per pixel) graphics. In a packed format, every *sequential* four bits represent a pixel. So the first byte of data is the first two pixels, the next byte is the next two pixels, and so on.

The concept of [planar graphics](https://en.wikipedia.org/wiki/Planar_(computer_graphics)) takes some understanding of binary to understand well. A pixel value is the sum of each set data bit. With that in mind, if we break out each of those bits and visualize them as "layers" that combine to form a value, we arrive at the concept of [bitplanes](https://en.wikipedia.org/wiki/Bit_plane). *A bitplane is the group of all digits at a given position.* Bitplane 0 is the group of bit 0 of all data in a tile; bitplane 1 is the group of bit 1 and so one. The sum of the digits at a certain pixel offset within each bitplane generates our final pixel color.

Let's break down some data. If we're working with 4bpp tiles, then each pixel has four bits. In an 8x8 tile, that's 64 pixels * 4bpp = 256 bits. That means there are 64 bits of data in bit 0 across all pixels; and 64 bits of data for bit 1, and for bit 2 and bit 3.

In packed format, bits 0 to 3 for each pixel are "packed" together, with each pixel stored sequentially.

    pixel number  pix0 pix1 pix2 pix3 pix4 ...
       pixel bit  3210 3210 3210 3210 3210 ...

In planar format, the values of bit 0 for all pixels are stored together, followed by the values of bit 1 for all pixels, and so on.

    pixel number pix0 pix1 pix2 ... pix0 pix1 pix2 ... pix0 pix1 pix2
      pixel bit     0    0    0 ...   1    1    1  ...  2    2    2

*In packed format, data is grouped by pixels; in planar format, data is grouped by bitplanes.*

The [Neo-Geo Development Wiki has an excellent visualization](https://wiki.neogeodev.org/index.php?title=Sprite_graphics_format) of planar data that may help if you're having trouble understanding the concept.

Ultimately, the different formats used on different hardware are variations on bitdepth and the arrangement of bitplanes/pixels. A tile definition used by chrgfx is a "map" of this arrangement used to decode the tile graphics.

### Color Data
RGB colors are stored in as many bytes as it takes to contain all the data bits for each component; this is between 1 and 3 bytes.

Since color data is operated on as a single data structure (8-bit byte, 16-bit short, 32-bit long), it is important to take into consideration the [endianness](https://en.wikipedia.org/wiki/Endianness) of the hardware you are working with because it will affect the order of the stored data.

For example, Super Famicom colors are 5 bit, so for all three color components, we have 15 bits. This takes up two bytes (16bits, with one of the bits unused). It looks something like this:

    15|       |8  7|       |0
      xBBBBBGG      GGGRRRRR

However, the SNES is a little endian system, which means the least significant bytes of a number are stored first. So in memory, the data appears like this:

    7|       |0  15|       |8
      GGGRRRRR      xBBBBBGG

It's important to specify if the color data is little or big endian so it can be properly decoded.

Note that a color data is only for devices which have colors with RGB components. Hardware that uses YIQ or other such non-digital colorspaces will have a pre-defined palette.

### Palette Data
Palettes appear as a simple sequential list of colors, where the first color is entry 0, the next is entry 1 and so on. With chrgfx, a palette is the system palette. We specify the size of subpalettes, and those subpalettes can be used to apply color to tiles.






The code is written to operate on the palette data as it is stored inside the machine's memory during runtime. Often this is stored inside the video RAM along with CHR and mapping data, though in some cases it has it's own discrete memory. In either case, the data needs to be extracted as a raw, binary chunk and stored in a file.

You will likely need to use an emulator that has a debugger capable of dumping memory, or at least viewing memory so bytes can be manually copied by hand. Some emulators store raw memory dumps in savestate files, which may be useful for obtaining palette data. Please see the Format Specific Notes below for information that may be useful to you.

Devices generally have a large 'system palette' which is subdivided into small palettes to be used for different objects and graphics on the screen. For example, the Sega Mega Drive has a 64 color system palette which is divided into four 16 color palettes. Since the Mega Drive CHR graphics are 4bpp, they can address, at most, 16 colors. If a tile uses colors from the third subpalette, that subpalette will need to be specified with the ```--subpalette``` option. Otherwise, it will always use the first subpalette and the colors will be incorrect.

Aside from device specific formats, there are also generic formats. Currently the only one implemented is the TileLayer Pro format. The colors in this palette can be applied to CHR data from any device.




## Special Thanks
Special thanks to Klarth for his venerable `consolegfx.txt`, which was my introduction to the data side of tiled graphics and was instrumental in my early development work in Dumpster and now chrgfx.
