# chrgfx
Converts tile (CHR) based bitmap graphics from retro hardware systems, similar to Tile Layer Pro and such.

## Supported formats
### Tiles
- Generic 1bpp (1bpp)
- Sega Megadrive/Sharp X68000 (sega_md)
- Sega 8-bit devices, e.g. Master Sytsem/SG-1000/Game Gear (sega_8bit)
- Nintendo Famicom/NES (nintendo_fc)
- Nintendo GameBoy & GameBoy Color (nintendo_gb)
- Nintendo Super Famicom/NEC PC Engine (nintendo_sfc)
- Capcom CPS1/CPS2 16x16 tiles (capcom_cps)
- SNK NeoGeo Pocket (snk_ngp)

### Palettes
- Tile Layer Pro (tilelayerpro)
- Sega Megadrive CRAM dump (sega_md)
- Sega Master System CRAM dump (sega_sms)
- Sega Game Gear CRAM dump (sega_gg)
- Nintendo Famicom/NES (nintendo_fc)
- Nintendo GameBoy (nintendo_gb & nintendo_gb_pocket)
- Nintendo GameBoy Color (nintendo_gb_color)
- Nintendo Super Famicom/SNES CGRAM dump (nintendo_sfc)
- SNK NeoGeo Pocket & Pocket Color (snk_ngp & snk_ngpc)

## Usage
```--chr-format```,```-f```

Specifies tile data format. Currently supported values are:

- 1bpp
- sega_md
- sega_8bit
- nintendo_fc
- nintendo_gb
- nintendo_sfc
- nintendo_vb
- capcom_cps
- snk_ngp

```--pal-format```,```-g```

Specifies the palette data format. Currently supported values are:

- tilelayerpro
- sega_md
- sega_sms
- sega_gg
- nintendo_fc
- nintendo_gb
- nintendo_gb_pocket
- nintendo_gb_color
- nintendo_sfc
- nintendo_vb
- snk_ngp
- snk_ngpc

Note: If the palette format identifier is the same as the CHR format identifier, you do not need to specify the palette format. I.E., if you use ```--chr-format sega_md``` you do not need to use ```--pal-format sega_md``` as well.

```--chr-data```,```-t```

Specifies a filename from which tile data will be read. If not specified, data will be read input from stdin.

```--pal-data```,```-p```

Specified a filename from which palette data will be read.

```--output```,```-o```

Specifies output PNG file to write to. If not specified, image will be written to stdout.

```--trns```,```-r```

Enable palette transparency. Default transparent index is 0.

```--trns-index```,```-i```

Specify which palette index to use as transparency

```--columns```,```-c```

Number of tile columns (horizontal) to render for output image.

```--subpalette```,```-s```

Specify the subpalette to be used from within the palette data. This is system specific.

### Usage Example
    cat ../etc/sonic_sprite | ./chrgfx --chr-format sega_md --pal-data ../etc/sonic1.cram --trns --columns 32 > test.png

## Format Specific Notes

### Nintendo Famicom palettes (nintendo\_fc)
The Famicom color set is "hardcoded" by the PPU. This system palette has 64 values available, though there are only 54 distinct colors due to quirks in how the hardware operates. CHR palettes consist of three colors which each  entry being 1 byte. However, palettes are 4 bytes in size total; the first byte is mirrored across all palettes and makes up the screen background color.

There are four background palettes and four sprite palettes available, which comes out to 32 bytes of CHR palettes. These are stored sequentially in the PPU memory starting at 0x3F00. These 32 bytes can be dumped to a file to make up palette data for chrgfx to work with.

Since the PPU generates NTSC signals directly instead of working with RGB values, colors must be approximated. Moreover, different revisions of the PPU have slightly different system palettes. The chrgfx converter uses rendition of the colors from the site below, based on the standard 2C02 PPU:

https://wiki.nesdev.com/w/index.php/PPU_palettes

### Nintendo GameBoy palettes (nintendo\_gb and nintendo\_gb\_pocket)
Original GameBoy colors are 2 bit, with 4 color palettes, making one palette 1 byte in size. There are three palettes in the system's PPU: BGRDPAL, OBJ0PAL, and OBJ1PAL. These three palettes are stored respectively within the system's IO port memory range, starting at 0xFF47 to 0xFF49.

The chrgfx GameBoy palette converters will use these three bytes to form a system palette. To create such a palette, simply copy the three bytes as shown in memory (by means of e.g. an emulator debugger) into a file and reference that file as the `--pal-data`.

There are two flavors of GameBoy palette converter: Original and Pocket. The only difference is the color shading. Original has a yellow-green tint, while the Pocket is more gray.

More info on the Gameboy hardware can be found here: https://fms.komkon.org/GameBoy/Tech/Software.html

### Nintendo GameBoy Color palettes (nintendo\_gb\_color)
GameBoy Color palettes consist of four 15-bit colors. There are 8 palettes for background graphics, and 8 palettes for sprites, for a total of 128 bytes of color RAM split across two buffers (for background and sprite palettes). The chrgfx converter will use one of these buffers; therefore the palette data should be 64 bytes in length.

### Nintendo VirtualBoy palettes (nintendo\_vb)
VirtualBoy palettes consist of 4 colors, each color being 2 bits, making a palette 1 byte in size. There are 8 palettes total: 4 for background, 4 for objects. They are stored at 0x5F860 in memory. Despite each palette being only 1 byte, there are 2 bytes allocated for each entry. The palette value is mirrored across both bytes. Altogether, with the byte mirroring, the full runtime palette is 16 bytes. The chrgfx palette converter takes in these 16 bytes as palette data.

### SNK NeoGeo Pocket palettes (snk\_ngp)
Palettes for the original, monochrome Pocket are made up 3 bit shades of gray. There are three system palettes, for Sprite, Scroll 1 and Scroll 2. Each palette has two subpalettes, each with four colors each. Each color is 1 byte: the lowest three bits are the color shade, while the rest are not used. The palette data is stored in memory starting at 0x8100, with 8 bytes for each of the system palettes.

The chrgfx palette converter will take in these 24 bytes as the palette data.
