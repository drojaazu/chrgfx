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
- Nintendo VirtualBoy (nintendo_vb)
- Capcom CPS1/CPS2 16x16 tiles (capcom_cps)
- SNK NeoGeo (snk_neogeo)
- SNK NeoGeo CD (snk_neogeocd)
- SNK NeoGeo Pocket (snk_ngp)

### Palettes
- Tile Layer Pro (tilelayerpro)
- Sega Megadrive (sega_md)
- Sega Master System (sega_sms)
- Sega Game Gear (sega_gg)
- Sega Puzzle Construction data file (sega_pzlcnst)
- Nintendo Famicom/NES (nintendo_fc)
- Nintendo GameBoy (nintendo_gb)
- Nintendo GameBoy Pocket (nintendo_gb_pocket)
- Nintendo GameBoy Color (nintendo_gb_color)
- Nintendo Super Famicom/SNES CGRAM dump (nintendo_sfc)
- Nintendo VirtualBoy (nintendo_vb)
- SNK NeoGeo (snk_neogeo)
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
- snk_neogeo
- snk_neogeocd
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
- snk_neogeo
- snk_ngp
- snk_ngpc

Note: If the palette format identifier is the same as the CHR format identifier, you do not need to specify the palette format. I.E., if you use ```--chr-format sega_md``` you do not need to use ```--pal-format sega_md``` as well. However, it's not smart enough to detect CHR variants that may use the same palette system. For example, if you use ```--chr-format nintendo_gb``` with palette data, you will need to manually specify ```--pal-format snk_neogeo```

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

Specify the subpalette to be used from within the palette data. This is system specific. The value is zero indexed.

### Usage Example
    cat ../etc/sonic_sprite | ./chrgfx --chr-format sega_md --pal-data ../etc/sonic1.cram --trns --columns 32 > test.png

## CHRs
A CHR is short for 'char,' which is short for character. This evolved from the world of programming, where the char data type is an unsigned 8-bit value. It was commonly used for holding single text characters. In early game development days, it became synonymous with the basic unit of graphics, the 8x8 tile.

This page has a pretty good introduction to the concept of 8x8 tiles and how old game systems displayed graphics: https://n3s.io/index.php?title=How_It_Works

While CHRs are traditionally 8x8 pixels in size, I use the term to refer to any sized graphics tile (16x16, 32x32, etc), just for convenience's sake.

## Palettes
Color palette data varies wildly across systems. The code is written to operate on the palette data as it is stored inside the machine's memory during runtime. Often this is stored inside the video RAM along with CHR and mapping data, though in some cases it has it's own discrete memory. In either case, the data needs to be extracted as a raw, binary chunk and stored in a file.

You will likely need to use an emulator that has a debugger capable of dumping memory, or at least viewing memory so bytes can be manually copied by hand. Some emulators store raw memory dumps in savestate files, which may be useful for obtaining palette data. Please see the Format Specific Notes below for information that may be useful to you.

Devices generally have a large 'system palette' which is subdivided into small palettes to be used for different objects and graphics on the screen. For example, the Sega Megadrive has a 64 color system palette which is divided into four 16 color palettes. Since the Megadrive CHR graphics are 4bpp, they can address, at most, 16 colors. If a tile uses colors from the third subpalette, that subpalette will need to be specified with the ```--subpalette``` option. Otherwise, it will always use the first subpalette and the colors will be incorrect.

Aside from device specific formats, there are also generic formats. Currently the only one implemented is the TileLayer Pro format. The colors in this palette can be applied to CHR data from any device.

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

### SNK NeoGeo and GeoGeo CD CHR data (snk\_neogeo & snk\_neogeocd)
The NeoGeo MVS is a somewhat unique system in that tile data is not loaded into VRAM, but rather addressed directly from it's ROM. You will need to the use dumps of the C ROMs. Moreover, you will need to interleave the odd and even dumps into one file in order to use the data in chrgfx. Interleave odd first then even, at two bytes each.

More info on how C ROM data is split can be found here: https://wiki.neogeodev.org/index.php?title=Sprite_graphics_format

CHR data for NeoGeo CD is stored in the SPR files on the disc. It can be processed as-is with the ```snk_neogeocd``` format.

### SNK NeoGeo palette (snk\_neogeo)
The NeoGeo system palette is a bit unique. It is made up of 256 16-color palettes, yielding 4096 (!) colors in the system palette. Since the maximum amount of colors in an indexed image is 256, we cannot export all 4096 colors in one image. Therefore, the code will stop processing data once it hits 256 colors in the output palette.

However, any of the 256 subpalettes may be addressed, which is a preferable method anyway.

The total size for the system palette is 8192 bytes (exactly 8KB). This data starts at 0x400000 in memory. The code will operate on all 8KB of data.

More info on NeoGeo color palettes can be found here: https://wiki.neogeodev.org/index.php?title=Palettes

### SNK NeoGeo Pocket palettes (snk\_ngp)
Palettes for the original, monochrome Pocket are made up 3 bit shades of gray. There are three system palettes, for Sprite, Scroll 1 and Scroll 2. Each palette has two subpalettes, each with four colors each. Each color is 1 byte: the lowest three bits are the color shade, while the rest are not used. The palette data is stored in memory starting at 0x8100, with 8 bytes for each of the system palettes.

The chrgfx palette converter will take in these 24 bytes as the palette data.

### Sega Puzzle Construction data files (sega\_pzlcnst)
Puzzle Construction is an application for the Sega Teradrive allowing users to design puzzles on the PC side and play them on the Megadrive side. Many of the data files for the PC application (in the PZL diretory) are stored as Mega Drive CHR patterns with the palette at the beginning of the file. The palette is slightly different from the native CRAM format. The colors are still 3 bit but are split across three bytes as R/G/B rather than being packed into two bytes.

Since chrgfx expects palette and CHR data to be in seperate files, you will need to manually split the data. The format is very simple: the first 0x100 bytes are palette data (though only up to 0xc0 is valid, since the Mega Drive memory for 64 colors in CRAM) while the rest that follows is pattern data. You can split these into two files, or make use of a tool like dd to split up data over a pipe:
```dd bs=$((0x100)) skip=1 if=MENU.PTN | chrgfx --chr-format=sega_md --pal-format=sega_pzlcnst --pal-data=MENU.PTN --columns=29 > pzlcnst_menu.png```

