# Included gfxdef files
This directory contains gfxdef files for a number of formats. Most contain both tile and color/palette definitions, but in cases where only one of the two are present a note will be made.

A couple of important things to remember. First, *you can mix and match gfxdef files with the `--chr-def` and `--pal-def` options.* For example, the Sega Game Gear and Master System/Mark III share the same tile format, but have different color formats. Rather than duplicating the tile definition, we specify it in the Game Gear file only. We can then decode Master System data like this:

    --chr-def sega_gg.gfxdef --pal-def sega_ms.gfxdef

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
