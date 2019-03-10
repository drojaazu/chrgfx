# chrgfx
---
Converts tile (CHR) based bitmap graphics from retro hardware systems, similar to Tile Layer Pro and such.

**NOTE**
This is early in development! It is part of rewrite of my old Dumpster/dumplib tools in C++, and is just a side project right now.

## Supported formats
### Tiles
- Generic 1bpp (1bpp)
- Sega Megadrive/X68000 (sega_md)
- Sega 8-bit devices, e.g. Master Sytsem/SG-1000/Game Gear (sega_8bit)
- Nintendo Famicom/NES (nintendo_fc)
- Nintendo Super Famicom/NEC PC Engine (nintendo_sfc)
- Capcom CPS1/CPS2 16x16 tiles (capcom_cps)

### Palettes
- Tile Layer Pro (tilelayerpro)
- Sega Megadrive CRAM dump (sega_md)
- Sega Master System CRAM dump (sega_sms)
- Sega Game Gear CRAM dump (sega_gg)
- Nintendo Super Famicom/SNES CGRAM dump (nintendo_sfc)

## Usage
```--chr-format```,```-f```
Specifies tile data format. Currently supported values are:
- 1bpp
- sega_md
- sega_8bit
- nintendo_fc
- nintendo_sfc
- capcom_cps

```--pal-format```,```-g```
Specifies the palette data format. Currently supported values are:
- tilelayerpro
- sega_md
- sega_sms
- sega_gg
- nintendo_sfc

Note: If the palette format identifier is the same as the tile format identifier, you do not need to specify the palette format. I.E., if you use ```--chr-format sega_md``` you do not need to use ```--pal-format sega_md``` as well.

```--chr-data```,```-t```
Specifies a filename from which tile data will be read. If not specified, will read input from stdin.

```--pal-data```,```-p```
Specified a filename from which palette data will be read.

```--output```,```-o```
Specifies output PNG file to write to. If not specified, will write to stdout.

```--trns```,```-r```
Use palette index 0 as transparency in output image.

```--trns-entry```,```-e```
Specify which palette entry to use as transparency

```--columns```,```-c```
Number of tile columns (horizontal) to render for output image.

```--pal-offset```,```-s```
Number of source palette entries to skip. For example, if source palette has 256 colors, but the 16 color palette for your sprite starts on palette "line" 3, the offset would be 48.

### Usage Example
    cat ../etc/sonic_sprite | ./chrgfx --chr-format sega_md --pal-data ../etc/sonic1.cram --trns --columns 32 > test.png

##