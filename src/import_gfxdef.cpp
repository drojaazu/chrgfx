#include "import_gfxdef.hpp"

namespace chrgfx
{
void parse_gfxdef(std::string filename, const chr_def* chrdef = nullptr,
									const pal_def* paldef = nullptr)
{
	// this is all kind of a mess, and there's probably a better way to do things,
	// but gotta start somewhere...
	std::ifstream infile(filename, std::ifstream::in);
	std::map<const std::string, std::string> chrdef_opts, paldef_opts;

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
				{defopts::PAL_SUBPAL_LENGTH, ""},	 {defopts::PAL_SUBPAL_COUNT, ""},
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
	std::array<u32, MAX_CHR_PLANES> chrdef_temp_planeoffset = {};
	std::array<u32, MAX_CHR_SIZE> chrdef_temp_xoffset = {},
																chrdef_temp_yoffset = {};

	int paldef_temp_colorsize = -1, paldef_temp_subpal_length = -1,
			paldef_temp_subpal_count = -1, paldef_temp_red_shift = -1,
			paldef_temp_red_bitcount = -1, paldef_temp_green_shift = -1,
			paldef_temp_green_bitcount = -1, paldef_temp_blue_shift = -1,
			paldef_temp_blue_bitcount = -1;
	bool paldef_temp_big_endinan = false;
	palette* paldef_temp_syspal = nullptr;

	while(std::getline(infile, this_line))
	{
		std::transform(this_line.begin(), this_line.end(), this_line.begin(),
									 tolower);
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
	}
	// end of while file read loop
}

void validate_chrdef_opts(std::map<const std::string, std::string>* chrdef_opts,
													const chr_def* chrdef)
{
	// check that all required options have a value
	if(chrdef_opts->at(defopts::CHR_WIDTH).empty() ||
		 chrdef_opts->at(defopts::CHR_HEIGHT).empty() ||
		 chrdef_opts->at(defopts::CHR_BPP).empty() ||
		 chrdef_opts->at(defopts::CHR_PLANEOFFSET).empty() ||
		 chrdef_opts->at(defopts::CHR_XOFFSET).empty() ||
		 chrdef_opts->at(defopts::CHR_YOFFSET).empty())
	{
		throw "One or more required options missing in chrdef";
	}

	/*
		width: greater than 0, eq/lt MAX_CHR_SIZE
		height: greater than 0, eq/lt MAX_CHR_SIZE
		bpp: greater than 0, eq/lt MAX_CHR_PLANES
		planeoffset: array length must match bpp, values greater than 0
		xoffset: array length must match width, values greater than 0
		yoffset: array length must match height, values grater than 0
	 */

	u16 chr_width_temp = str_validate_ispos(chrdef_opts->at(defopts::CHR_WIDTH));
	u16 chr_height_temp =
			str_validate_ispos(chrdef_opts->at(defopts::CHR_HEIGHT));
	u16 chr_bpp_temp = str_validate_ispos(chrdef_opts->at(defopts::CHR_BPP));

	std::vector<u32> temp = str_validate_array_vals_ispos_length(
			chrdef_opts->at(defopts::CHR_PLANEOFFSET), chr_bpp_temp);
	std::array<u32, MAX_CHR_PLANES> chr_planeoffset_temp;
	std::move(temp.begin(), temp.end(), chr_planeoffset_temp.begin());

	temp = str_validate_array_vals_ispos_length(
			chrdef_opts->at(defopts::CHR_XOFFSET), chr_width_temp);
	std::array<u32, MAX_CHR_SIZE> chr_xoffset_temp;
	std::move(temp.begin(), temp.end(), chr_xoffset_temp.begin());

	temp = str_validate_array_vals_ispos_length(
			chrdef_opts->at(defopts::CHR_YOFFSET), chr_height_temp);
	std::array<u32, MAX_CHR_SIZE> chr_yoffset_temp;
	std::move(temp.begin(), temp.end(), chr_yoffset_temp.begin());

	chrdef = new chr_def(chr_width_temp, chr_height_temp, chr_bpp_temp,
											 chr_planeoffset_temp, chr_xoffset_temp, chr_yoffset_temp,
											 get_chr);
}

void validate_paldef_opts(std::map<const std::string, std::string>* paldef_opts,
													const pal_def* paldef)
{
	/*
		colorsize: greater than 0
		subpal_length: greater than 0
		subpal_count: greater than 0
		syspal: optionally defined, but if not defined, color calc must be set
		passes: greater than 0, eq/lt MAX_COLOR_PASSES
		shift: non negative
		bitcount: greater than 0
	 */

	// VALIDATE PALDEF HERE
	// if any colordef options are set, then ALL must be set
	// if both syspal and colordef are undefined, error
	// if both syspal and colordef are defined, issue warning that syspal will
	// be used
	bool use_syspal = false;

	if(paldef_opts->at(defopts::PAL_SYSPAL).empty())
	{
		use_syspal = true;
	}

	// check that all required options have a value
	if(paldef_opts->at(defopts::PAL_COLORSIZE).empty() ||
		 paldef_opts->at(defopts::PAL_SUBPAL_LENGTH).empty() ||
		 paldef_opts->at(defopts::PAL_SUBPAL_COUNT).empty() ||
		 (!use_syspal & (paldef_opts->at(defopts::PAL_COLOR_PASSES).empty() ||
										 paldef_opts->at(defopts::PAL_RED_SHIFT).empty() ||
										 paldef_opts->at(defopts::PAL_RED_BITCOUNT).empty() ||
										 paldef_opts->at(defopts::PAL_GREEN_SHIFT).empty() ||
										 paldef_opts->at(defopts::PAL_GREEN_BITCOUNT).empty() ||
										 paldef_opts->at(defopts::PAL_BLUE_SHIFT).empty() ||
										 paldef_opts->at(defopts::PAL_BLUE_BITCOUNT).empty())))
	{
		throw "One or more required options missing in chrdef";
	}

	u8 colorsize_temp =
			str_validate_ispos(paldef_opts->at(defopts::PAL_COLORSIZE));
	u8 subpal_length_temp =
			str_validate_ispos(paldef_opts->at(defopts::PAL_SUBPAL_LENGTH));
	u8 subpal_count_temp =
			str_validate_ispos(paldef_opts->at(defopts::PAL_SUBPAL_COUNT));

	palette* syspal_temp = nullptr;
	color_def* colordef_temp = nullptr;
	if(use_syspal)
	{
		syspal_temp =
				str_validate_array_get_palette(paldef_opts->at(defopts::PAL_SYSPAL));
	}
	else
	{
		// validate calc color settings
		u32 color_passes_temp =
				str_validate_ispos(paldef_opts->at(defopts::PAL_COLOR_PASSES));
		std::array<u8, MAX_COLOR_PASSES> red_shift_temp, red_bitcount_temp,
				green_shift_temp, green_bitcount_temp, blue_shift_temp,
				blue_bitcount_temp;
		std::vector<u32> temp;

		temp =
				str_validate_array_vals_nonneg(paldef_opts->at(defopts::PAL_RED_SHIFT));
		std::move(temp.begin(), temp.end(), red_shift_temp.begin());

		temp = str_validate_array_vals_nonneg(
				paldef_opts->at(defopts::PAL_RED_BITCOUNT));
		std::move(temp.begin(), temp.end(), red_bitcount_temp.begin());

		temp = str_validate_array_vals_nonneg(
				paldef_opts->at(defopts::PAL_GREEN_SHIFT));
		std::move(temp.begin(), temp.end(), green_shift_temp.begin());

		temp = str_validate_array_vals_nonneg(
				paldef_opts->at(defopts::PAL_GREEN_BITCOUNT));
		std::move(temp.begin(), temp.end(), green_bitcount_temp.begin());

		temp = str_validate_array_vals_nonneg(
				paldef_opts->at(defopts::PAL_BLUE_SHIFT));
		std::move(temp.begin(), temp.end(), blue_shift_temp.begin());

		temp = str_validate_array_vals_nonneg(
				paldef_opts->at(defopts::PAL_BLUE_BITCOUNT));
		std::move(temp.begin(), temp.end(), blue_bitcount_temp.begin());

		colordef_temp = new color_def(
				color_passes_temp, red_shift_temp, red_bitcount_temp, green_shift_temp,
				green_bitcount_temp, blue_shift_temp, blue_bitcount_temp);
	}
	// todo: check is_big_endian
	bool is_big_endian_temp = true;

	if(use_syspal)
		paldef =
				new pal_def(colorsize_temp, subpal_length_temp, subpal_count_temp,
										nullptr, syspal_temp, pal_decode_fixed, is_big_endian_temp);
	else
		paldef = new pal_def(colorsize_temp, subpal_length_temp, subpal_count_temp,
												 colordef_temp, nullptr, pal_decode_calc,
												 is_big_endian_temp);
}

inline u32 str_validate_ispos(std::string value)
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

inline u32 str_validate_nonneg(std::string value)
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

	if(out < 0)
	{
		throw std::invalid_argument(
				"Value is not a valid number; must be 0 or greater");
	}

	return (u32)out;
}

inline std::vector<u32> str_validate_array_vals_nonneg(std::string value)
{
	std::vector<u32> out;
	std::stringstream ss(value);
	std::string this_value;
	try
	{
		while(std::getline(ss, this_value, ','))
		{
			out.push_back(str_validate_nonneg(this_value));
		}
	}
	catch(const std::invalid_argument& e)
	{
		throw std::invalid_argument("Invalid numeric value in array");
	}

	return out;
}

inline std::vector<u32> str_validate_array_vals_ispos_length(std::string value,
																														 s32 req_length)
{
	std::vector<u32> out;
	out.reserve(req_length);
	std::stringstream ss(value);
	std::string this_value;
	try
	{
		while(std::getline(ss, this_value, ','))
		{
			out.push_back(str_validate_ispos(this_value));
		}
	}
	catch(const std::invalid_argument& e)
	{
		throw std::invalid_argument("Invalid numeric value in array");
	}

	if(out.size() != req_length)
	{
		throw std::invalid_argument("Not enough elements in array");
	}

	return out;
}

inline palette* str_validate_array_get_palette(std::string value)
{
	palette* out = new palette();
	std::stringstream ss(value), hex_conv;
	std::string this_value;
	u8 red = 0, green = 0, blue = 0;
	try
	{
		while(std::getline(ss, this_value, ','))
		{
			if(this_value[0] == '#')
				this_value.erase(this_value.begin(), this_value.begin() + 1);
			if(this_value.length() != 6)
				throw std::invalid_argument("Invalid hex color in syspal definition");

			hex_conv << this_value.substr(0, 2);
			hex_conv >> red;
			hex_conv.clear();

			hex_conv << this_value.substr(2, 2);
			hex_conv >> green;
			hex_conv.clear();

			hex_conv << this_value.substr(4, 2);
			hex_conv >> blue;
			hex_conv.clear();

			out->push_back(color(red, green, blue));
		}
	}
	catch(const std::invalid_argument& e)
	{
		throw std::invalid_argument("Invalid color value in array");
	}

	return out;
}
}	 // namespace chrgfx
