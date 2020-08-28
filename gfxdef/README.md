# Graphics Definitions
Rather than hardcoding conversion routines for each hardware system, chrgfx uses generalized algorithms which support the great majority of tile based graphics hardware. The algorithms rely on layout information to interpret data, and that information comes from user-defined *graphics definitions* (gfxdef).

There are three "types" within a graphics definition: a tile definition (chrdef), a palette definition (paldef), and a color definition (coldef). There are also profile definitions which are a grouping of the previously mentioned types to represent a certain hardware system.

## gfxdefs File
Included with chrgfx is a collection of definitions for many well-known systems, including most of those from the 8 and 16-bit era. This file is meant to be used in conjunction with the `png2chr` and `chr2png` tools. By default, it should be installed to `/usr/local/lib/chrgfx/gfxdefs`.

The following hardware is included in the file. Use the `--profile` option to specify the hardware.

`nintendo_fc` - Nintendo Famicom / NES
`nintendo_sfc` - Nintendo Super Famicom / Super Nintendo (SNES)
`nintendo_vb` - Nintendo Virtual Boy
`nintendo_gb` - Nintendo Game Boy
`nintendo_gb_pocket` - Nintendo Game Boy Pocket
`sega_gg` - Sega Game Gear
`sega_ms` - Sega Master System / Mark III
`sega_md` - Sega Mega Drive / Genesis
`capcom_cps` - Capcom CPS-1/2
`snk_neogeo` - SNK Neo-Geo AES/MVS
`snk_neogeocd` - SNK Neo-Geo CD

## Relationship Between gfxdefs File and libchrgfx
As a point of clarification, the gfxdefs file is used only by the two frontend utilities. It is an abstraction of the chrdef, paldef and coldef classes used internally in libchrgfx. As such, while the rest of this document explains the details of these concepts and is useful for developers, it is explained in the context of an external file loaded at runtime. All of the key names here match the names of the members of each class in the code.

Those wishing to integrate libchrgfx should refer to the [Using the Library]() section of the main readme as well as the comments in the source code.

## Definition Format
A gfxdef is composed of a group up of key-value pairs, one per line, wrapped in C style braces with the type specified at the top. Keys are one word, while values are either a single or comma-delimited list of numbers. For example:

    chrdef
    {
      id chr_nintendo_fc
      comment Nintendo Famicom
    
      # 8x8 2bpp planar
      width 8
      height 8
      bpp 2
      planeoffset 0,64
      pixeloffset 0,1,2,3,4,5,6,7
      rowoffset 0,8,16,24,32,40,48,56
    }

The two keys shared across all types are `id` and `comment`. The `id` is the unique identifier for the definition. This should be unique in the group of the same type, so you could legally have a chrdef called "nintendo_fc" and a paldef also called "nintendo_fc," however, it's probably best to keep them all unique with a type prefix to prevent any confusion. The `comment` key is a freeform string with a more detailed description of the definition, which can be used for logging or identifying the type in a more user friendly way.

Lines prefixed eith `#` are treated as internal comments and are not processed.

### Tile Definitions (chrdef)
Tiles are derived from reading a "map" of the bit-level offset of bitplanes and pixels on the x and y axes. This concept originated from the GfxLayout structure in MAME.

Jumping right in to a relatively simple example: Sega Mega Drive tiles are 8x8 4bpp packed pixel format. Since it is packed, the bit data for each pixel is stored sequentially; therefore, the offset for bitplanes 0 to 3 is 0,1,2,3. Since each pixel is four bits and the pixels are stored sequentially, the offset for each pixel moving horizontally (i.e. each pixel in a row) is 0,4,8,12,16,20,24,28. Since each row is 8 pixels * 4bpp, each row is 32 bits in size. Thus, the data offset for each row moving vertically, top to bottom, is 0,32,64,96,128,160,192,224.

Note that the number of entries in the bitplane offset list must match the number of bitplanes. In the above example, there are four bitplanes (4bpp), so there are four entries in the bitplane offset list. Similarly, the number of entries in the X offset must match the pixel width, and the Y offset count must match the height.

Our Mega Drive tile definition ends up looking like this:

    chrdef
    {
      id chr_sega_md
      comment Sega Megadrive
      width 8
      height 8
      bpp 4
      planeoffset 3,2,1,0
      pixeloffset 0,4,8,12,16,20,24,28
      rowoffset 0,32,64,96,128,160,192,224
    }

Nintendo Super Famicom tiles are a bit more complex. They are 8x8 4bpp planar format, with bitplanes 0 and 1 grouped together followed by bitplanes 2 and 3. Each byte (8 bits) represents a bitplane for the row. The first 16 bytes make up bitplanes 0 and 1 of each row, while the last 16 bytes make up bitplanes 2 and 3. This layout from Klarth's consolegfx document helps visualize the data (where each pair in brackets is a byte of data and r = row, bp = bitplane):

    [r0, bp0], [r0, bp1], [r1, bp0], [r1, bp1], [r2, bp0], [r2, bp1], [r3, bp0], [r3, bp1]
    [r4, bp0], [r4, bp1], [r5, bp0], [r5, bp1], [r6, bp0], [r6, bp1], [r7, bp0], [r7, bp1]
    [r0, bp2], [r0, bp3], [r1, bp2], [r1, bp3], [r2, bp2], [r2, bp3], [r3, bp2], [r3, bp3]
    [r4, bp2], [r4, bp3], [r5, bp2], [r5, bp3], [r6, bp2], [r6, bp3], [r7, bp2], [r7, bp3]

As a chrdef, it looks like this:

    chrdef
    {
      id chr_nintendo_sfc
      comment Nintendo Super Famicom, NEC PC Engine
      width 8
      height 8
      bpp 4
      # bp0 is first, then bp1 is 1 byte (8 bits) later
      # then bp2 starts 16 bytes (128 bits) from the start of the pixel, with bp3 one byte after that
      planeoffset 0,8,128,136
      pixeloffset 0,1,2,3,4,5,6,7
      rowoffset 0,16,32,48,64,80,96,112
    }

### chrdef Reference

`width`, `height` - The pixel dimensions of a single tile

`bpp` - The number of bits per pixel (the bitdepth)

`planeoffset` - The offset (in bits) of each bitplane in one pixel; **the number of entries here must match the value of `bpp`.**

`xoffset` - The offset (in bits) of each pixel within a row (x axis); **the number of entries here must match the value of `width`.**

`yoffset` - The offset (in bits) of each pixel row within a tile (y axis); **the number of entries here must match the value of `height`.**

`chr_converter` - (Optional) Specifies the internal conversion function to use; **this is only necessary for custom conversion functions in non-standard formats, and shouldn't normally be needed.**

### Palette Definitions (paldef)
When we think of a palette, we generally envision a structure that contains a list of colors. This is still the case, but in chrgfx we split up the concept of palette structure and color data. If you imagine a carton of a dozen colorful Easter eggs, the eggs are the colors (coldef) while the carton holding them is the palette (paldef).

Generally, the graphics hardware holds a finite number of colors from which the tiles on the screen may select a subset to use. We call the total group of colors the "system palette" or "full palette" while the subset is called a "subpalette" or "palette line."

Returning to the Sega Mega Drive again as a simple example: its hardware has a system palette of 64 colors, while each subpalette is 16 colors. This means there are 4 subpalettes within the full system palette, and the tile graphics can use any one of those four subpalettes.

Something to note, however, is that the indexed PNGs (i.e. PNGs using a palette) such as those output from `chr2png` have a maximum of 256 colors. This is not a problem for most systems, though some may have a full palette with more than 256 entries. (For example, the Neo-Geo hardware, with 256 4bit subpalettes, a whopping 4,096 colors!) In such cases, the palette is cut off at the 256 color mark. Instead of a full palette, though, you can specify any subpalette to apply a palette line for entries past the cutoff.

### paldef reference

`subpal_length` - The number of colors in one subpalette

`subpal_count` - The number of subpalettes in the system palette

`entry_datasize` - The size of a single entry within a palette

`subpal_datasize` - (Optional) The size 

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

`subpal_datasize` - (Optional) Manually specify the size of a subpalette (in bits). Used for cases where effective subpalette data is less than 8 bits, but is stored in 2 or more bytes. Default is 0 (automatic).

`big_endian` - (Optional) Indicates the system is big endian rather than small endian. 1 is true, 0 is false. Default is 0 (little endian).

`refpal` - An array of HTML formatted (e.g. #001E74) colors to simulate output of non-RGB hardware. The entries can be listed with or without the hash symbol (Either 001E74 or #001E74 is acceptable.)

`color_passes` - Specifies the number of times the shift/extra process will be performed on color data. Optional; default is 1. **Note that if this value is higher than 1, then EACH of below shift/size settings MUST have a number of entries equal to color_passes.**

`red_shift` - The number of bit places to shift the red component data into the LSB.

`green_shift` - The number of bit places to shift the green component data into the LSB.

`blue_shift` - The number of bit places to shift the blue component data into the LSB.

`red_size` - The number of bits of data making up the red component.

`green_size` - The number of bits of data making up the green component.

`blue_size` - The number of bits of data making up the blue component.

`pal_converter` - (Optional) Specifies the internal conversion function to use. **This is only necessary for custom conversion functions in non-standard formats, and shouldn't normally be needed.**



Second, *some formats are supported by multiple systems.* For example, the Super Famicom 4bpp format is also used by the NEC PC Engine. The command line options for chrgfx are long enough, and we don't want to make it more ugly by naming the gfxdef files with every system they support (nintendo_sfc_nec_pcengine.gfxdef would be pretty awful). If you don't see a system you want to work with, check this readme first or grep through the files for a keyword to see which files match.

Filenames are listed without the .gfxdef extension.

## Standard formats
`capcom_cps_16`

Hardware: Capcom CPS and CPS-2 (arcade)
This is for 16x16 tiles.

`generic_1bpp` *(chrdef ONLY)*

Hardware: any
This is a generic format that is probably not actually used by any systems, but is provided as a theoretical possibility. 1bpp tiles would mean each pixel is one bit, meaning two colors only.

`nintendo_fc`

Hardware: Nintendo Famicom / NES

`nintendo_gb`

Hardware: Nintendo Gameboy, Gameboy Color (tiles only), Super Famicom / SNES (tiles only)

`nintendo_gbc` *(paldef ONLY)*

Hardware: Nintendo Gameboy Color

`nintendo_gbp` *(paldef ONLY)*

Hardware: Gameboy Pocket
Actually has the same palette format as the original Gameboy, but with a different refpal for the different screen tint.

`nintendo_sfc`

Hardware: Nintendo Super Famicom / SNES, NEC PC Engine (tiles only)

`nintendo_vb`

Hardware: Nintendo Virtual Boy

`sega_gg`

Hardware: Sega Game Gear, Sega Master System / Mark III (tiles only), Bandai Wonderswan (tiles only)

`sega_md`

Hardware: Sega Mega Drive, Sharp X68000 (tiles only)

`sega_ms` *(paldef ONLY)*

Hardware: Sega Master System / Mark III

`seta_16_4bpp`

Hardware: Seta arcade hardware
This is for 16x16 4bpp BG layer tiles.

`snk_neogeo`

Hardware: SNK Neo-Geo, SNK Neo-Geo CD (tiles only)

`snk_neogeocd` *(chrdef ONLY)*

Hardware: SNK Neo-Geo CD

`snk_neogeopocket`

Hardware: SNK Neo-Geo Pocket, Neo-Geo Pocket Color (tiles only)

`snk_neogeopocketcolor` *(paldef ONLY)*

Hardware: SNK Neo-Geo Pocket Color

## Non-standard formats
These are formats that use custom conversion functions. The list is small right now, but more are planned.

`palette_tilelayerpro`

Uses a TileLayer Pro .TPL file for the color palette. Only RGB format palette supported (but check the gfxdef file for notes, as the other modes can be used with the native gfxdefs). The subpalette size can be modified as needed in the gfxdef file.
