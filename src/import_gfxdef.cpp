#include "import_gfxdef.hpp"

namespace chrgfx
{
void parse_gfxdef(std::ifstream* infile, const chr_def* chrdef = nullptr,
									const pal_def* paldef = nullptr)
{
	// this is all kind of a mess, and there's probably a better way to do things,
	// but gotta start somewhere...
	std::map<const std::string, std::string> chrdef_opts, paldef_opts;
	if(infile == nullptr) throw "No handle to input gfxdef file";

	if(chrdef == nullptr & paldef == nullptr)
		throw "chrdef and paldef cannot both be undefined when reading gfxdef file";

	bool use_chrdef = false, use_paldef = false;
	if(chrdef != nullptr)
	{
		use_chrdef = true;
		std::initializer_list<std::pair<const std::string, std::string>> temp_il = {
				{defopts::CHR_WIDTH, ""},		{defopts::CHR_HEIGHT, ""},
				{defopts::CHR_BPP, ""},			{defopts::CHR_PLANEOFFSET, ""},
				{defopts::CHR_XOFFSET, ""}, {defopts::CHR_YOFFSET, ""}};
		chrdef_opts.insert(temp_il);
	}

	if(paldef != nullptr)
	{
		use_paldef = true;
		std::initializer_list<std::pair<const std::string, std::string>> temp_il = {
				{defopts::PAL_COLORSIZE, ""},			 {defopts::PAL_COLOR_PASSES, ""},
				{defopts::PAL_SUBPAL_LENGTH, ""},	{defopts::PAL_SUBPAL_COUNT, ""},
				{defopts::PAL_SYSPAL, ""},				 {defopts::PAL_RED_SHIFT, ""},
				{defopts::PAL_RED_BITCOUNT, ""},	 {defopts::PAL_GREEN_SHIFT, ""},
				{defopts::PAL_GREEN_BITCOUNT, ""}, {defopts::PAL_BLUE_SHIFT, ""},
				{defopts::PAL_BLUE_BITCOUNT, ""}};
		paldef_opts.insert(temp_il);
	}

	int linenumber = 0;
	std::string this_line, this_key, this_val;
	std::size_t spacedelim_pos;
	const char space = ' ';
	int chrdef_temp_width = -1, chrdef_temp_height = -1, chrdef_temp_bpp = -1;
	std::array<u32, MAX_GFX_PLANES> chrdef_temp_planeoffset = {};
	std::array<u32, MAX_GFX_SIZE> chrdef_temp_xoffset = {},
																chrdef_temp_yoffset = {};

	int paldef_temp_colorsize = -1, paldef_temp_subpal_length = -1,
			paldef_temp_subpal_count = -1, paldef_temp_red_shift = -1,
			paldef_temp_red_bitcount = -1, paldef_temp_green_shift = -1,
			paldef_temp_green_bitcount = -1, paldef_temp_blue_shift = -1,
			paldef_temp_blue_bitcount = -1;
	bool paldef_temp_big_endinan = false;
	palette* paldef_temp_syspal = nullptr;

	while(std::getline(*infile, this_line))
	{
		linenumber++;
		if(this_line[0] == '#')
		{
			// ignore comment lines
			continue;
		}

		// split string at first space
		spacedelim_pos = this_line.find(space);
		if(spacedelim_pos == std::string::npos)
		{
			throw std::string("Invalid entry in gfxdef file at line " + linenumber);
		}

		this_key = this_line.substr(0, spacedelim_pos);
		this_val = this_line.substr(spacedelim_pos, std::string::npos);

		// identify setting
		// chrdef specific settings
		try
		{
			if(use_chrdef)
			{
				if(chrdef_opts.find(this_key) != chrdef_opts.end())
					chrdef_opts[this_key] = this_val;

				/*
					const u16 width;		 // pixel width of each element
		const u16 height;		 // pixel height of each element
		const u8 bitplanes;	// number of color bitplanes
		const std::array<u32, MAX_GFX_PLANES>
				planeoffset;	// bit offset of each bitplane
		const std::array<u32, MAX_GFX_SIZE>
				xoffset;	// bit offset of each horizontal pixel
		const std::array<u32, MAX_GFX_SIZE>
				yoffset;	// bit offset of each vertical pixel
		const u8 *(*converter)(const chr_def *, const u8 *);
		const u32 datasize;	// size of one chr in bits
				*/
			}

			if(use_paldef)
			{
				if(paldef_opts.find(this_key) != paldef_opts.end())
					paldef_opts[this_key] = this_val;
			}
		}
		catch(const std::exception& e)
		{
			throw std::string("Invalid value while processing gfxdef file on line " +
												linenumber);
		}

		// VALIDATE CHRDEF HERE

		// VALIDATE PALDEF HERE
		// if any colordef options are set, then ALL must be set
		// if both syspal and colordef are undefined, error
		// if both syspal and colordef are defined, issue warning that syspal will
		// be used
	}
	// end of while file read loop

	// PASS CHRDEF/PALDEF OPTS TO VALIDATORS
	// the end
}

void validate_chrdef_opts(std::map<const std::string, std::string>* chrdef_opts,
													const chr_def* chrdef)
{
	/*
						if(this_key == defopts::CHR_WIDTH)
						{
							// width cannot be 0 or neg
						}
						if(this_key == defopts::CHR_HEIGHT)
						{
							// height cannot be 0 or neg
						}
						if(this_key == defopts::CHR_BPP)
						{
							// bpp cannot be 0 or neg
						}
						if(this_key == defopts::CHR_PLANEOFFSET)
						{
							// array, count must match bpp
						}
						if(this_key == defopts::CHR_XOFFSET)
						{
							// array, count must match width
						}
						if(this_key == defopts::CHR_YOFFSET)
						{
							// array, count must match height
						} */
}

void validate_paldef_opts(std::map<const std::string, std::string>* paldef_opts,
													const pal_def* paldef)
{
	/*	try
			{
				// paldef specific settings
				if(use_paldef)
				{
					if(this_key == defopts::PAL_COLORSIZE)
					{
						// cannot be 0 or neg
					}
					if(this_key == defopts::PAL_SUBPAL_LENGTH)
					{
						// cannot be 0 or neg
					}
					if(this_key == defopts::PAL_SUBPAL_COUNT)
					{
						// cannot be 0 or neg
					}
					if(this_key == defopts::PAL_SYSPAL)
					{
						// array, count cannot be 0
					}
					if(this_key == defopts::PAL_BIG_ENDIAN)
					{
						// boolean, optional, defaults to false
					}
					if(this_key == defopts::PAL_COLOR_PASSES)
					{
						// cannot be 0 or neg
						// optional, but if set other color calc settings must be set
					}
					if(this_key == defopts::PAL_RED_SHIFT)
					{
						// cannot be neg
						// optional, but if set other color calc settings must be set
					}
					if(this_key == defopts::PAL_RED_BITCOUNT)
					{
						// cannot be 0 or neg
						// optional, but if set other color calc settings must be set
					}
					if(this_key == defopts::PAL_GREEN_SHIFT)
					{
						// cannot be neg
						// optional, but if set other color calc settings must be set
					}
					if(this_key == defopts::PAL_GREEN_BITCOUNT)
					{
						// cannot be 0 or neg
						// optional, but if set other color calc settings must be set
					}
					if(this_key == defopts::PAL_BLUE_SHIFT)
					{
						// cannot be neg
						// optional, but if set other color calc settings must be set
					}
					if(this_key == defopts::PAL_BLUE_BITCOUNT)
					{
						// cannot be 0 or neg
						// optional, but if set other color calc settings must be set
					}
				}
			}
			catch(const std::exception& e)
			{
				throw std::string("Invalid value while processing paldef file on line "
			+ linenumber);
			} */
}

u32 str_validate_ispos(std::string value)
{
	int out = -1;
	try
	{
		out = std::stoi(value);
	}
	catch(const std::invalid_argument& e)
	{
		throw std::invalid_argument("Value is not a valid number string");
	}

	if(out < 1)
		throw std::invalid_argument(
				"Value is not a valid number; must be 1 or greater");

	return (u32)out;
}
}	// namespace chrgfx
