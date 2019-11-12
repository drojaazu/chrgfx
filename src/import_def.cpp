#include "import_def.hpp"

// this is all kinds of a mess, and maybe there's a better way to do
// things, but gotta start somewhere...

#define DEFOPT(optname) def_opts.at(defopts::optname)

namespace chrgfx
{
kvmap parse_file(std::ifstream &infile)
{
	if(!infile.good()) {
		throw std::invalid_argument("File handle invalid");
	}

	infile.seekg(std::ios::beg);

	// map of all possible definition keys
	kvmap def_opts = {{defopts::CHR_WIDTH, ""},
										{defopts::CHR_HEIGHT, ""},
										{defopts::CHR_BPP, ""},
										{defopts::CHR_PLANEOFFSET, ""},
										{defopts::CHR_XOFFSET, ""},
										{defopts::CHR_YOFFSET, ""},
										{defopts::PAL_COLORSIZE, ""},
										{defopts::PAL_COLOR_PASSES, ""},
										{defopts::PAL_SUBPAL_LENGTH, ""},
										{defopts::PAL_SUBPAL_COUNT, ""},
										{defopts::PAL_REFPAL, ""},
										{defopts::PAL_RED_SHIFT, ""},
										{defopts::PAL_RED_SIZE, ""},
										{defopts::PAL_GREEN_SHIFT, ""},
										{defopts::PAL_GREEN_SIZE, ""},
										{defopts::PAL_BLUE_SHIFT, ""},
										{defopts::PAL_BLUE_SIZE, ""},
										{defopts::PAL_BIG_ENDIAN, ""},
										{defopts::PAL_SUBPAL_DATASIZE, ""}};

	int linenumber = 0;
	std::string this_line, this_key, this_val;
	std::size_t spacedelim_pos;
	// const char space = ' ';

	while(std::getline(infile, this_line)) {
		std::transform(this_line.begin(), this_line.end(), this_line.begin(),
									 tolower);
		linenumber++;
		if(this_line[0] == '#' || this_line == "") {
			// ignore comment & empty lines
			continue;
		}

		// split string at first space
		spacedelim_pos = this_line.find(' ');
		if(spacedelim_pos == std::string::npos) {
			throw std::invalid_argument("Invalid entry in gfxdef file at line " +
																	std::to_string(linenumber));
		}

		this_key = this_line.substr(0, spacedelim_pos);
		this_val = this_line.substr(spacedelim_pos + 1, std::string::npos);

		// remove any spaces from the value
		this_val.erase(std::remove(this_val.begin(), this_val.end(), ' '),
									 this_val.end());

		// identify setting
		if(def_opts.find(this_key) != def_opts.end())
			def_opts[this_key] = this_val;
#if DEBUG
		else
			std::cerr << "Unknown key value in gfxdef: " << std::endl << this_key;
#endif
	}

	// end of file read loop
	return def_opts;
}

chr_def *get_chrdef(std::ifstream &infile)
{
	std::map<const std::string, std::string> def_opts = parse_file(infile);

	// check that all required options have a value
	if(DEFOPT(CHR_WIDTH).empty() || DEFOPT(CHR_HEIGHT).empty() ||
		 DEFOPT(CHR_BPP).empty() || DEFOPT(CHR_PLANEOFFSET).empty() ||
		 DEFOPT(CHR_XOFFSET).empty() || DEFOPT(CHR_YOFFSET).empty()) {
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

	u16 chr_width_temp = str_validate_ispos(DEFOPT(CHR_WIDTH));
	u16 chr_height_temp = str_validate_ispos(DEFOPT(CHR_HEIGHT));
	u16 chr_bpp_temp = str_validate_ispos(DEFOPT(CHR_BPP));

	std::vector<u32> temp = str_validate_array_vals_nonneg_length(
			DEFOPT(CHR_PLANEOFFSET), chr_bpp_temp);
	std::array<u32, MAX_CHR_PLANES> chr_planeoffset_temp;
	std::move(temp.begin(), temp.end(), chr_planeoffset_temp.begin());

	temp = str_validate_array_vals_nonneg_length(DEFOPT(CHR_XOFFSET),
																							 chr_width_temp);
	std::array<u32, MAX_CHR_SIZE> chr_xoffset_temp;
	std::move(temp.begin(), temp.end(), chr_xoffset_temp.begin());

	temp = str_validate_array_vals_nonneg_length(DEFOPT(CHR_YOFFSET),
																							 chr_height_temp);
	std::array<u32, MAX_CHR_SIZE> chr_yoffset_temp;
	std::move(temp.begin(), temp.end(), chr_yoffset_temp.begin());

	return new chr_def(chr_width_temp, chr_height_temp, chr_bpp_temp,
										 chr_planeoffset_temp, chr_xoffset_temp, chr_yoffset_temp,
										 get_chr);
}

pal_def *get_paldef(std::ifstream &infile)
{
	std::map<const std::string, std::string> def_opts = parse_file(infile);

	/*
	colorsize: greater than 0
	subpal_length: greater than 0
	subpal_count: greater than 0
	refpal: optionally defined, but if not defined, color calc must be set
	passes: greater than 0, eq/lt MAX_COLOR_PASSES
	shift: non negative
	bitcount: greater than 0
 */

	// VALIDATE PALDEF HERE
	// if any colordef options are set, then ALL must be set
	// if both refpal and colordef are undefined, error
	// if both refpal and colordef are defined, issue warning that refpal will
	// be used
	bool use_refpal = false;

	if(!DEFOPT(PAL_REFPAL).empty()) {
		use_refpal = true;
	}

	// check that all required options have a value
	if(DEFOPT(PAL_COLORSIZE).empty() || DEFOPT(PAL_SUBPAL_LENGTH).empty() ||
		 DEFOPT(PAL_SUBPAL_COUNT).empty() ||
		 (!use_refpal &
			(DEFOPT(PAL_COLOR_PASSES).empty() || DEFOPT(PAL_RED_SHIFT).empty() ||
			 DEFOPT(PAL_RED_SIZE).empty() || DEFOPT(PAL_GREEN_SHIFT).empty() ||
			 DEFOPT(PAL_GREEN_SIZE).empty() || DEFOPT(PAL_BLUE_SHIFT).empty() ||
			 DEFOPT(PAL_BLUE_SIZE).empty()))) {
		throw std::invalid_argument(
				"One or more required options missing in paldef");
	}

	u8 colorsize_temp = str_validate_ispos(DEFOPT(PAL_COLORSIZE));
	u8 subpal_length_temp = str_validate_ispos(DEFOPT(PAL_SUBPAL_LENGTH));
	u8 subpal_count_temp = str_validate_ispos(DEFOPT(PAL_SUBPAL_COUNT));

	u8 subpal_datasize_temp =
			DEFOPT(PAL_SUBPAL_DATASIZE).empty()
					? 0
					: str_validate_nonneg(DEFOPT(PAL_SUBPAL_DATASIZE));

	palette *refpal_temp = nullptr;
	color_def *colordef_temp = nullptr;
	if(use_refpal) {
		refpal_temp = str_validate_array_get_palette(DEFOPT(PAL_REFPAL));
	} else {
		// validate calc color settings
		u32 color_passes_temp = str_validate_ispos(DEFOPT(PAL_COLOR_PASSES));
		std::array<u8, MAX_COLOR_PASSES> red_shift_temp, red_bitcount_temp,
				green_shift_temp, green_bitcount_temp, blue_shift_temp,
				blue_bitcount_temp;
		std::vector<u32> temp;

		temp = str_validate_array_vals_nonneg(DEFOPT(PAL_RED_SHIFT));
		std::move(temp.begin(), temp.end(), red_shift_temp.begin());

		temp = str_validate_array_vals_nonneg(DEFOPT(PAL_RED_SIZE));
		std::move(temp.begin(), temp.end(), red_bitcount_temp.begin());

		temp = str_validate_array_vals_nonneg(DEFOPT(PAL_GREEN_SHIFT));
		std::move(temp.begin(), temp.end(), green_shift_temp.begin());

		temp = str_validate_array_vals_nonneg(DEFOPT(PAL_GREEN_SIZE));
		std::move(temp.begin(), temp.end(), green_bitcount_temp.begin());

		temp = str_validate_array_vals_nonneg(DEFOPT(PAL_BLUE_SHIFT));
		std::move(temp.begin(), temp.end(), blue_shift_temp.begin());

		temp = str_validate_array_vals_nonneg(DEFOPT(PAL_BLUE_SIZE));
		std::move(temp.begin(), temp.end(), blue_bitcount_temp.begin());

		colordef_temp = new color_def(
				color_passes_temp, red_shift_temp, red_bitcount_temp, green_shift_temp,
				green_bitcount_temp, blue_shift_temp, blue_bitcount_temp);
	}

	bool is_big_endian_temp =
			DEFOPT(PAL_BIG_ENDIAN).empty() ? true : str_bool(DEFOPT(PAL_BIG_ENDIAN));

	if(use_refpal)
		return new pal_def(colorsize_temp, subpal_length_temp, subpal_count_temp,
											 nullptr, refpal_temp, pal_decode_fixed,
											 is_big_endian_temp, subpal_datasize_temp);
	else
		return new pal_def(colorsize_temp, subpal_length_temp, subpal_count_temp,
											 colordef_temp, nullptr, pal_decode_calc,
											 is_big_endian_temp, subpal_datasize_temp);
}

inline bool str_bool(std::string value)
{
	try {
		int temp = std::stoi(value);
		if(temp > 0)
			return true;
		else
			return false;
	} catch(const std::invalid_argument &e) {
#ifdef DEBUG
		std::cerr << "str_bool: Invalid value: " << value << std::endl;
#endif
		throw std::invalid_argument(
				"Value is not valid for true/false (use 0 = false, 1 = true)");
	}
}
inline u32 str_validate_ispos(std::string value)
{
	int out = -1;
	try {
		out = std::stoi(value);
	} catch(const std::invalid_argument &e) {
#ifdef DEBUG
		std::cerr << "str_validate_ispos: Invalid value: " << value << std::endl;
#endif
		throw std::invalid_argument("Value is not a valid number string");
	}

	if(out < 1) {
#ifdef DEBUG
		std::cerr << "str_validate_ispos: Invalid value: " << value << std::endl;
#endif
		throw std::invalid_argument(
				"Value is not a valid number; must be 1 or greater");
	}

	return (u32)out;
}

inline u32 str_validate_nonneg(std::string value)
{
	int out = -1;
	try {
		out = std::stoi(value);
	} catch(const std::invalid_argument &e) {
#ifdef DEBUG
		std::cerr << "str_validate_nonneg: Invalid value: " << value << std::endl;
#endif
		throw std::invalid_argument("Value is not a valid number string");
	}

	if(out < 0) {
#ifdef DEBUG
		std::cerr << "str_validate_nonneg: Invalid value: " << value << std::endl;
#endif
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
	try {
		while(std::getline(ss, this_value, ',')) {
			out.push_back(str_validate_nonneg(this_value));
		}
	} catch(const std::invalid_argument &e) {
		throw std::invalid_argument("Invalid numeric value in array");
	}

	return out;
}

inline std::vector<u32> str_validate_array_vals_nonneg_length(std::string value,
																															s32 req_length)
{
	std::vector<u32> out;
	out.reserve(req_length);
	std::stringstream ss(value);
	std::string this_value;
	try {
		while(std::getline(ss, this_value, ',')) {
			out.push_back(str_validate_nonneg(this_value));
		}
	} catch(const std::invalid_argument &e) {
		throw std::invalid_argument("Invalid numeric value in array");
	}

	if(out.size() != req_length) {
		throw std::invalid_argument("Not enough elements in array (expected " +
																std::to_string(req_length) + ", got " +
																std::to_string(out.size()) + ")");
	}

	return out;
}

inline palette *str_validate_array_get_palette(std::string value)
{
	palette *out = new palette();
	// remove any spaces from the value before processing
	value.erase(std::remove(value.begin(), value.end(), ' '), value.end());

	std::stringstream ss(value);
	//, hex_conv;
	std::string this_value;
	u8 red = 0, green = 0, blue = 0;
	try {
		while(std::getline(ss, this_value, ',')) {
			if(this_value[0] == '#')
				this_value.erase(this_value.begin(), this_value.begin() + 1);
			if(this_value.length() != 6)
				throw std::invalid_argument("Invalid hex color in refpal definition");

			red = std::stoi(this_value.substr(0, 2), nullptr, 16);
			green = std::stoi(this_value.substr(2, 2), nullptr, 16);
			blue = std::stoi(this_value.substr(4, 2), nullptr, 16);

			out->push_back(color(red, green, blue));
		}
	} catch(const std::invalid_argument &e) {
		throw std::invalid_argument("Invalid color value in array");
	}

	return out;
}
} // namespace chrgfx

#undef DEFOPT