
# Retro Graphics Primer
The below is provided for those who are new to tile based graphics or who want clarification on some of the concepts around which chrgfx is designed.

## Tile (aka CHR)
There are a number of ways that video games end up being displayed. There are vectors, in which elements are drawn based on on the math of their geometry. There are polygons, the basis for 3D gameplay that is now commonplace. There are LCD screens like the Game & Watch and Tiger handhelds, which have predrawn art that is displayed or cleared by electrical signals. And there are tile based games, most commonly used in the 2D arcade and home console hardware from the late 70's through the 90's. It is on the data from these tile based games that chrgfx is intended to operate.

The basic unit of graphics in these games is the tile. Tiles are essentially very small bitmaps, where a bitmap is a 2D plane of dots (pixels) with each dot having one color. These tiles combine much like a jigsaw puzzle to form the larger backgrounds and sprites and, ultimately, full image on the screen. Tiles are generally 8 by 8 pixels in size, though in later systems they were commonly 16x16 or larger.

An old term for a graphics tile is "CHR," which is short for "char," which is short for "character." This evolved from the world of programming, where the char data type is an unsigned 8-bit (1 byte) value. It was commonly used for holding text characters. In early game development days, it became synonymous with the tile data, since the limited size of graphics storage meant that tiles coule be reference with just one byte, a char type. It is commonly associated with the Nintendo Famicom's CHR graphics data bank, though other contemporaneous hardware used similar terminology.

CHR is not really used as a term anymore, though I chose it for the program name to match the retro dev aesthetic. :)

## Colors
In the digital grapics sense, a color is composed of levels of red, green and blue mixed together, which our eyes perceive as a certain hue. The amount of data allocated to each of these color levels is measured in bits; this is known as color depth. Modern photographic images are 24 bit, with 8 bits each for R, G and B levels. That gives us more than 16 million distinct colors to work with.

Older game hardware did not have such a large gamut available. The number of colors that could be generated was dependent on the video hardware in each system. For example, the Sega Mega Drive has 9 bit colors (3 bits each for R, G and B), which can generate up to 512 unique colors. Super Famicom colors are 15 bit (32,769 colors). And the original Gameboy is 2 bit, with a maximum of 4 colors.

(To be clear, the bit depth of colors has nothing to do with the video game advertising of the 1990's, which heralded the Super Famicom/Sega Mega Drive/etc as "16 bit," and the next generations as "32 bit" and "64 bit." That advertising was intentionally ambiguous ("bigger number = better graphics!"), but it actually refers to the width of the data bus, which allowed more data to be addressable and thus more actual game content present. It was not directly related to graphics, per se.)

Some early consoles did not use digital RGB, and instead generated their colors based on television broadcast signal standards, such as the Atari 2600 and the original Nintendo Famicom. Or perhaps there are only levels of brightness in a monochrome display, such as the Gameboy. Since such colors are inherently dependent on the output hardware, we cannot 100% accurately recreate these digitally. Moreover, there may be multiple hardware revisions that change color output minutely. (Once again, the Famicom and Gameboy are the best examples here: [There is much discussion about the color output of the various Famicom PPU revisions](https://wiki.nesdev.org/w/index.php/PPU_palettes), and while the Gameboy and Gameboy Pocket have essentially the same hardware, the screen on the original has a notably green tint, while the latter is a light gray.)

In order to represent such devices, we create predefined, "best guess" equivalent RGB colors stored in a hardcoded reference palette. With this method, we can easily create multiple palettes for a system to represent its hardware variations (e.g. either a Gameboy original palette or a Gameboy Pocket palette applied to the same data).

## Palettes
There are two main methods for storing the colors of a bitmap: direct and indexed. In direct color mode, *each pixel specifies its own red, green and blue levels*. This is used almost exclusively in our modern digital world, where data size is not so much of a concern, used in photography, computers, modern video games, and practically anything involving a digital display. Direct color mode, however, was not often used by retro hardware until towards the 32 bit era and onward.

In indexed color mode, colors themselves are divorced from pixels and stored as a seperate list. As a result, *each pixel specifies an entry in that color list*. This seperate color list is known as a color palette. Indexed color images are smaller in data size than direct color since each pixel only needs enough bits to reference one of those palette entries. The tradeoff here is that the color list is small, usually 256 color maximum. This means indexed mode is no good for things like photographs, which need a full range of color to reproduce what we see. But it works great for imagery with a narrow range of colors, like cartoon artwork and pixelated video game graphics.

It's important to understand the distinction between colors *possible* and colors *available.* As we said in the previous section, the Super Famicom has 15bit color, meaning 32,769 colors are *possible.* However, its color palette only has 256 entries, which means only 256 colors are *available* to be displayed at any one time. And the graphics are 4 bits per pixel, meaning there are a maximum of 16 colors available in the tile. 

## Graphics As Data
So now that we have explained things at a theoretical level, let's look at how these concepts are implemented in data. It's important to have a basic understanding of how the data is stored and arranged when attempting to create graphics definitions.

### Tile Data
Since we already know the dimensions of tile and since the color/palette data is stored elsewhere, tile data boils down to simply an array of pixels, nothing more.

The data layout of those pixels is organized in one of two ways: packed or planar. [Packed pixel](https://en.wikipedia.org/wiki/Packed_pixel) (also called linear or "chunky") format is the more intuitive of the two. In this style, *a single plixel is represented as sequential data*. Let's say we're working with 4bpp (bits per pixel) graphics. In a packed format, every *sequential* four bits represent a pixel. So the first byte of data is the first two pixels, the next byte is the next two pixels, and so on.

The concept of [planar graphics](https://en.wikipedia.org/wiki/Planar_(computer_graphics)) takes some rudimentary knowledge of binary to fully understand. First, understand that, regardless of packed or planar format, a single pixel is the sum of each set bit within its data. With that in mind, if we break out each of the bit positions and visualize them as "layers" that combine to form a value, we arrive at the concept of [bitplanes](https://en.wikipedia.org/wiki/Bit_plane). *A bitplane is the group of all digits at a given bit position.* Bitplane 0 is the grouping of all bit 0 of all data in a tile; bitplane 1 is the group of bit 1 and so one. The sum of the digits at a certain pixel offset within each bitplane generates our final pixel color.

Let's break down some data. If we're working with 4bpp tiles, then each pixel has four bits. In an 8x8 tile, that's 64 pixels * 4bpp = 256 bits. That means there are 64 bits of data in bit 0 across all pixels; and 64 bits of data for bit 1, and for bit 2 and bit 3.

In packed format, bits 0 to 3 for each pixel are "packed" together, with each pixel stored sequentially.

    pixel number  pix0 pix1 pix2 pix3 pix4 ...
       pixel bit  3210 3210 3210 3210 3210 ...

In planar format, the values of bit 0 for all pixels are stored together, followed by the values of bit 1 for all pixels, and so on.

    pixel number pix0 pix1 pix2 ... pix0 pix1 pix2 ... pix0 pix1 pix2 ...
       pixel bit  3    3    3   ...  2    2    2   ...  1    1    1   ...

*In packed format, data is grouped by pixels; in planar format, data is grouped by bitplanes.*

The [Neo-Geo Development Wiki has an excellent visualization](https://wiki.neogeodev.org/index.php?title=Sprite_graphics_format) of planar data that may help if you're having trouble understanding the concept.

Ultimately, the different formats used on different hardware are variations on bitdepth and the arrangement of bitplanes/pixels. A tile encoding used by chrgfx is a "map" of this arrangement, used to decode/encode the tile graphics.

### Color Data
RGB colors are stored in as many bytes as it takes to contain all the data bits for each component; this is between 1 and 3 bytes.

Since color data is operated on as a single data structure (8-bit byte, 16-bit short, 32-bit long), it is important to take into consideration the [endianness](https://en.wikipedia.org/wiki/Endianness) of the hardware you are working with because it will affect the order of the stored data.

For example, Super Famicom colors are 5 bit, so for all three color components, we have 15 bits. This takes up two bytes (16bits, with one of the bits unused). It looks something like this:

    15|       |8  7|       |0
      xBBBBBGG      GGGRRRRR

However, the SNES is a little endian system, which means the least significant bytes of a number are stored first. So in data storage, the data appears like this:

    7|       |0  15|       |8
      GGGRRRRR      xBBBBBGG

It's important to specify if the color data is little or big endian so it can be properly processed on the local machine.

(It's also important to note that endianness does not matter while in memory. Data is always stored with most significant bits to the "left" and less significant bits to the "right." Endianness only matters as data stored in a file, with which we are obviously working.)

Note that a color data is only for devices which have colors with RGB components. Hardware that uses YIQ or other such non-RGB colorspaces will need a pre-defined palette of color approximations.

### Palette Data
A palette is simply an array of all the color values, ordered seqentially. So a palette of 64 entries of 16 bit colors would be 128 bytes in size, with color 0 at offset 0, color 1 at offset 2, and so on. There are some quirks on some systems (such as the Virtual Boy, which only uses 6 bits for a palette yet is stored in 2 bytes...), but they are rare. Palette data is just an array of the color values we previously discussed.
