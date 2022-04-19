# Graphics Definitions
Rather than hardcoding conversion routines for each hardware system, chrgfx uses generalized algorithms which support the great majority of tile based graphics hardware. The algorithms rely on layout information to interpret data, and that information comes from user-defined *graphics definitions* (gfxdef).

There are three "types" within a graphics definition: a tile definition (chrdef), a palette definition (paldef), and a color definition (coldef). There are also profile definitions which are a grouping of the previously mentioned types to represent a certain hardware system.

## gfxdefs File
Included with chrgfx is a collection of definitions for many well-known systems, including most of those from the 8 and 16-bit era. This file is meant to be used in conjunction with the `png2chr` and `chr2png` tools. By default, it should be installed to `/etc/chrgfx/gfxdefs`.

The following hardware is included in the file. Use the `--profile` option to specify the hardware.

`nintendo_fc` - Nintendo Famicom / NES

`nintendo_sfc` - Nintendo Super Famicom / Super Nintendo (SNES)

`nintendo_vb` - Nintendo Virtual Boy

`nintendo_gb` - Nintendo Game Boy

`nintendo_gb_pocket` - Nintendo Game Boy Pocket

`nec_pcengine_sprites` - NEC PC Engine / TurboGrafx-16 (sprites)

`nec_pcengine_tiles` - NEC PC Engine / TurboGrafx-16 (tiles)

`sega_gg` - Sega Game Gear

`sega_ms` - Sega Master System / Mark III

`sega_md` - Sega Mega Drive / Genesis

`capcom_cps` - Capcom CPS-1/2

`snk_neogeo` - SNK Neo-Geo AES/MVS

`snk_neogeocd` - SNK Neo-Geo CD

## Relationship Between gfxdefs File and libchrgfx
As a point of clarification, the gfxdefs file is used only by the two frontend utilities, `chr2png` and `png2chr`. It is an abstraction of the chrdef, paldef and coldef classes used internally in libchrgfx. As such, while the rest of this document delves into these concepts and is useful for developers, it is explained in the context of an external file loaded at runtime.

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

`pixeloffset` - The offset (in bits) of each row of pixels within a tile; **the number of entries here must match the value of `height`.**

`rowoffset` - The offset (in bits) of each pixel within a row; **the number of entries here must match the value of `width`.**

### Palette Definitions (paldef)
When we think of a palette, we generally envision a structure that contains a list of colors. This is still the case, but in chrgfx we split up the concept of palette structure and color data. If you imagine a carton of a dozen colorful Easter eggs, the eggs are the colors (coldef) while the carton holding them is the palette (paldef). Splitting of color data and palette structure gives us more flexibility and less redundancy, which we'll see later when we talk about profiles.

### paldef reference

`pal_length` - The number of colors in the palette

`entry_datasize` - The size of a single entry within a palette, in bits

`pal_datasize` - (Optional) The data size of a single palette, in bits. This should only be needed in very rare circumstances where the size of a subpalette is greater than the sum of the color data. An example of this is the Nintendo Virtual Boy.

### Color Definitions (coldef)
Colors are derived in one of two ways. The first method calculates a color using RGB values specified by the size and position of each color channel within the data. This works for hardware that natively uses RGB colorspace. For hardware which does not, the second method is used. This involves mapping all possible colors to approximate RGB values in a reference table.

Here is an example of an RGB definition using the Super Famicom:

    coldef
    {
      id col_nintendo_sfc
      comment Nintendo Super Famicom
    
      # 15|               |0
      #   xBBBBBGG GGGRRRRR
      bitdepth 5
      color_passes 1
      red_shift 0
      red_size 5
      green_shift 5
      green_size 5
      blue_shift 10
      blue_size 5
    
      big_endian 0
    }

The basic algorithm works by shifting the channel bitsand masking the component data. Therefore we specify the size of the data for each component and its position relative to the least significant bit. You can visualize this with the bit layout in the comments above. There are five bits of red, which are natually positioned at the LSB, so the shift is 0. There are five bits of green, which need to be shifted 5 bits to the right to arrive at the LSB. Finally, blue is 5 bits as well, shifted 10 bits to the right.

This is fine for simple layouts with single blocks of sequential data, like the Super Famicom, but let's look at the definition for the Neo-Geo next:

    rgbcoldef
    {
      id col_snk_neogeo
      comment SNK NeoGeo
    
      # 15 |                                   |0
      #    D R0 G0 B0 R4R3R2R1 G4G3G2G1 B4B3B2B1
      # D = "dark bit", acts as shared LSB for all color components
      bitdepth 6
      color_passes 3
      red_shift 15,14,8
      red_size 1,1,4
      green_shift 15,13,4
      green_size 1,1,4
      blue_shift 15,12,0
      blue_size 1,1,4
      big_endian 1
    }

Here we introduce the concept of passes. Each pass will process the whole data, and the bits extracted each time are combined from the LSB upwards to form the final color information. In this example with Neo-Geo color, we see that the channels are not entirely packed together: there is an extra bit for each channel in the upper part of the word. Moreover, there is a so-called "dark bit" which acts as the shared least significant bit for each channel.

To parse data in this format, we need to make three passes: first for the "dark bit" acting as the lowest bit for each channel, then the single bit in the upper byte, and finally the packed data making up the rest of the color information. We thus indicate the sizes and shifts for each of these passes in sequential order for each color channel. The dark bit ends up in bit 0, the dangling low bit ends up in bit 1, and the remaining data ends up in bits 2 to 5, forming our complete 6 bit channel data.

It is imperative that each color channel size/shift specifiers have a number of entries matching the amount of color passes. Also, if the color data is more than one byte in size, the endianness of the hardware should be specified to ensure compatibility.

Reference tables are much easier to understand and define. As an example, the original Nintendo Famicom uses the YIQ color space and encodes the colors directly into the TV signal. With 64 colors available, we need to provide an RGB value that approximates the original perceived color for each of those entries.

    refcoldef
    {
      id col_nintendo_fc
      comment Nintendo Famicom
    
      big_endian 0

      # a 'standard' 2C02 PPU palette, from:
      # https://wiki.nesdev.com/w/index.php/PPU_palettes
      reftab #545454,#001E74,#08102C,#300088,#440064,#5C0030,#540400,...
    }

(The full list of 64 colors was abridged here for readability.)

The first entry will correspond to value 0, the next to value 1, and so on.

### coldef reference

big_endian - (Optional) Indicates the original hardware is big endian; if not specified, default is 0 (false). The value should be either 1 (true, big endian) or 0 (false, little endian). This should be specified for hardware where color data is greater than 8 bits in size.

#### rgbcoldef reference

`bitdepth` - The size of each color component, in bits

`color_passes` - (Optional) Indicates the number of times the value will be passed; if not specified, the value will be 1. **If the value is greater than 1, each of the `*_shift` and `*_size` entries below must have a comma delimited list with a count of entries that match this setting.**

`red_shift` - Specifies the number of spaces to shift the red component data to the right, into the LSB

`green_shift` - As above, for green component data

`blue_shift` - As above, for blue component data

`red_size` - The size of the red component data, in bits

`green_size` - As above, for green component data

`blue_size` - As anove, for blue component data

#### refcoldef reference

`reftab` - A comma delimited list of HTML style RGB colors to represent each possible non-RGB color on the original hardware
