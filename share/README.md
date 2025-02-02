# Graphics Definitions

Rather than hardcoding conversion routines for each hardware system, chrgfx uses generalized algorithms which can support the great majority of tile based graphics hardwareby using bit layout information to interpret the data. That layout information comes from user-defined *graphics definitions* (gfxdefs).

There are three types of graphics definitions: tile definitions (chrdef), palette definitions (paldef), and color definitions (coldef). There are also profile definitions which are a grouping of the previously mentioned types to represent a certain hardware system.

# gfxdefs File

Included with chrgfx is a collection of definitions for many well-known systems, including most of those from the 8 and 16-bit era. This file is meant to be used in conjunction with the `png2chr` and `chr2png` tools. By default, it is installed to `/usr/share/chrgfx/gfxdefs` or `/usr/share/local/chrgfx/gfxdefs`.

## Relationship Between gfxdefs File and libchrgfx

As a point of clarification, the gfxdefs file is used only by the support utilities, `chr2png`, `png2chr`, and `palview`. It is a collection of serializations of chrdef, paldef and coldef classes used internally in libchrgfx.

As such, while the rest of this document delves into these concepts and is useful for developers, it is explained in the context of an external file loaded at runtime. Developers planning to use libchrgfx in their applications should instead consider the following in the context of the respective classes.

# Definition Format and Shared Keys

A gfxdef is composed of a group up of key-value pairs, one per line, wrapped in braces with the type specified at the top. Keys are one word, delimited by a space, while the value can be any format on one line after it. For example:

    chrdef
    {
      id chr_nintendo_fc
      desc Nintendo Famicom
    
      # 8x8 2bpp planar
      width 8
      height 8
      bpp 2
      pixel_offsets 0,1,2,3,4,5,6,7
      row_offsets 0,8,16,24,32,40,48,56
      plane_offsets 0,64
    }

The two keys shared across all types are `id` and `desc`. The `id` is the unique identifier for the definition. This should be unique in the group of the same type, so you could legally have a chrdef called "nintendo_fc" and a paldef also called "nintendo_fc," however, it's probably best to keep them all unique with a type prefix to prevent any confusion. The `desc` key is a freeform string with a more detailed description of the definition, which can be used for logging or identifying the type in a more user friendly way.

Lines prefixed eith `#` are treated as internal comments and are not processed.

## Tile Definitions (chrdef)

Tiles are derived from reading a "map" of the bit-level offset of bitplanes, pixels on a row, and rows in the whole tile. This concept originated from the GfxLayout structure in MAME.

Jumping right in to a relatively simple example: Sega Mega Drive tiles are 8x8 4bpp packed pixel format: 8 pixels in width, 8 pixels in height, 4 bits of data per pixel. The data bits for each pixel are either grouped together (packed) or by plane (planar).

Since here it is packed, the bit data for each pixel is stored sequentially; therefore, the offset for bitplanes 0 to 3 is 0,1,2,3. Since each pixel is four bits and the 8 pixels are stored sequentially, the offset for each pixel moving horizontally (i.e. each pixel in a row) is 0,4,8,12,16,20,24,28. Since each row is 8 pixels * 4bpp, each row is 32 bits in size. Thus, the data offset for each row moving vertically, top to bottom, is 0,32,64,96,128,160,192,224.

Basically, we specify the layout for each "level" of data in the structure of a tile. First, offset to the start of each bitplane in a single pixel; then the offset to the start of each pixel in a single row; then the offsset to the start of each row in the whole tile.

Therefore, there is a relationship between bpp and plane_offsets; width and pixel_offsets; and height and row_offsets. THe number of entries for the latter must equal the value of the former. For example, a width of 8 must have 8 numbers listed in the pixel_offsets.

Our Mega Drive tile definition ends up looking like this:

    chrdef
    {
      id chr_sega_md
      desc Sega Megadrive
      width 8
      height 8
      bpp 4
      pixel_offsets 0,4,8,12,16,20,24,28
      row_offsets 0,32,64,96,128,160,192,224
      plane_offsets 3,2,1,0
    }

Nintendo Super Famicom tiles are a bit more complex. They are 8x8 4bpp planar format, with bitplanes 0 and 1 grouped together followed by bitplanes 2 and 3. Each byte (8 bits) represents a bitplane for the row. The first 16 bytes make up bitplanes 0 and 1 of each row, while the last 16 bytes make up bitplanes 2 and 3. This layout from Klarth's consolegfx document helps visualize the data (where each pair in brackets is a byte of data and r = row, bp = bitplane):

    [r0, bp0], [r0, bp1], [r1, bp0], [r1, bp1], [r2, bp0], [r2, bp1], [r3, bp0], [r3, bp1]
    [r4, bp0], [r4, bp1], [r5, bp0], [r5, bp1], [r6, bp0], [r6, bp1], [r7, bp0], [r7, bp1]
    [r0, bp2], [r0, bp3], [r1, bp2], [r1, bp3], [r2, bp2], [r2, bp3], [r3, bp2], [r3, bp3]
    [r4, bp2], [r4, bp3], [r5, bp2], [r5, bp3], [r6, bp2], [r6, bp3], [r7, bp2], [r7, bp3]

As a chrdef, it looks like this. Notice how the `plane_offsets` has an irregular layout:

    chrdef
    {
      id chr_nintendo_sfc
      desc Nintendo Super Famicom
      width 8
      height 8
      bpp 4
      # bp0 is first, then bp1 is 1 byte (8 bits) later
      # then bp2 starts 16 bytes (128 bits) from the start of the pixel, with bp3 one byte after that
      pixel_offsets 0,1,2,3,4,5,6,7
      row_offsets 0,16,32,48,64,80,96,112
      plane_offsets 0,8,128,136
    }

### chrdef Reference

`width`, `height` - The pixel dimensions of a single tile

`bpp` - The number of bits per pixel (the bitdepth)

`pixel_offsets` - The offset (in bits) of each row of pixels within a tile; **the number of entries here must match the value of `height`.**

`row_offsets` - The offset (in bits) of each pixel within a row; **the number of entries here must match the value of `width`.**

`plane_offsets` - The offset (in bits) of each bitplane in one pixel; **the number of entries here must match the value of `bpp`.**

## Palette Definitions (paldef)

When we think of a palette, we generally envision a structure that contains an ordered list of colors. This is still the case, but in chrgfx we split up the concept of palette structure and color data. If you imagine a carton of a dozen colorful Easter eggs, the eggs are the colors (coldef) while the carton holding them is the palette (paldef). Splitting of color data and palette structure gives us more flexibility and less redundancy, which we'll see later when we talk about profiles.

### paldef reference

`length` - The number of colors in the palette

`entry_datasize` - The size of a single entry within a palette *in bits*

`datasize` - (Optional) The data size of a single palette *in bits*. This should only be needed in very rare circumstances where the size of a subpalette is greater than the sum of the color data. An example of this is the Nintendo Virtual Boy.

## Color Definitions (coldef)

Colors are derived in one of two ways: RGB color channel bit mapping or predefined color lists.

### RGB colors (rgbcoldef)

The first method calculates a color using RGB values specified by the size and position of each color channel within the data. This works for hardware that natively uses RGB colorspace, which is the majority.

Here is an example of an RGB definition using the Super Famicom:

    coldef
    {
      id col_nintendo_sfc
      desc Nintendo Super Famicom
    
      # 15|              |0
      #   xBBBBBGGGGGRRRRR
      bitdepth 5
      layout 0,5,5,5,10,5
    }

The primary key here is `layout`, which defines the start offset (reletaive to the least-significant bit) and the number of bits for each color channel. It is a rigidly defined type, which will always have 6 entries:

```red_offset, red_datasize, green_offset, green_datasize, blue_offset, blue_datasize```

Note that the offset here is zero indexed.

In the comment within the example above, we see that the red data occupies bits 0 to 4; green data is at bits 5 to 9; blue data is at bits 10 to 14. Therefore, we specify red starts at bit offset 0, and is 5 bits long; green starts at bit offset 5 and is 5 bits long; blue starts at bit offset 10 and is 5 bits long.

This is fine for simple layouts with single blocks of sequential data, like the Super Famicom, but let's look at the definition for the Neo-Geo next:

    rgbcoldef
    {
      id col_snk_neogeo
      desc SNK NeoGeo
    
      # 15 |               |               |0
      #    DDR0G0B0R4R3R2R1 G4G3G2G1B4B3B2B1
      # D = "dark bit", acts as shared LSB for all color components
      bitdepth 6
      layout 15,1,15,1,15,1
      layout 14,1,13,1,12,1
      layout 8,4,4,4,0,4
      big_endian 1
    }

There are multiple `layout` entries here, and this is because the color format is a bit more complex. Looking at the example in the comment, we see bit 0 of each color channel occupying bits 12 to 14, while bits 1 to 4 of each channel are in bits 0 to 11. Moreover, there is a shared bit across all channels at bit 15.

So, we must break this up into three parts, one for each grouping: the shared bit, bit 0, and bits 1 to 4. Ultimately, we want each color channel isolated, so each bit must be extracted and positioned in the correct order for that channel.

Note that *the order of the layout entries affects the output bit position.* That is, *the order of the layout entries is important.* (This is only true for layouts; the order of configuration entries anywhere else in the `gfxdefs` file does not matter.)

The layout order is relative to the LSB. So if a color channel has 1 bit in size in the first layout, it will occupy bit 0 in the output. If the same color channel in the next layout as 3 bits in size, it will occupy bits 1 to 3 in the output. And so on.

In the example above, the "dark bit" acts as a shared LSB (taht is, bit 0) for all channels. It is in bit 15 in the input color, and is one bit. We specify that same value for all three channels since it is shared. Next is the lowest bit for the unique value on each channel, in bit 14 for red, 13 for green and 12 for blue. It is also 1 bit in size. Finally, the remaining bits are packed together and grouped by channel, with red at bit 8, green at bit 4 and blue at bit 0, each of which are 4 bits in size.

`big_endian` indicates that the source data is in big endian format (little endian is default). Color data is often stored as a 16- or 32-bit type, so we must account for the source hardware architecture. When considering the bit layout, always visualize it in big-endian, with the MSB on the left and and the LSB on the right.

#### rgbcoldef reference

`bitdepth` - The size of each color component, in bits

`layout` - The offset and count of bits for each color channel. Must have exactly 6 entries, consisting of: red offset, red size, green offset, green size, blue offset, blue size. 

`big_endian` - (Optional) Indicates the original hardware is big endian; if not specified, default is 0 (false). The value should be either 1 (true, big endian) or 0 (false, little endian). This should be specified for hardware where color data is greater than 8 bits in size.


### Color Tables (refcoldef)

For hardware which does not (usually old or very simple systems), the second method is used. This involves mapping all possible colors to approximate RGB values in a color map.

Reference color tables are much easier to understand and define. As an example, the original Nintendo Famicom uses the YIQ color space and encodes the colors directly into the TV signal. Colors are not defined by the software, but rather chosen from a color palette created by the system. With 64 colors available, we need to provide an RGB value that approximates the original perceived color for each of those entries.

    refcoldef
    {
      id col_nintendo_fc
      comment Nintendo Famicom

      # a 'standard' 2C02 PPU palette, from:
      # https://wiki.nesdev.com/w/index.php/PPU_palettes
      refpal #545454,#001E74,#08102C,#300088,#440064,#5C0030,#540400,...

      big_endian 0
    }

(The full list of 64 colors was abridged here for readability.)

The first entry will correspond to value 0, the next to value 1, and so on.

#### refcoldef reference

`refpal` - A comma delimited list of HTML style RGB colors to represent each possible color on the original hardware

`big_endian` - (Optional) Indicates the original hardware is big endian; if not specified, default is 0 (false). The value should be either 1 (true, big endian) or 0 (false, little endian). This should be specified for hardware where color data is greater than 8 bits in size.
