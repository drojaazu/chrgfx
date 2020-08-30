
#include "import_defs.hpp"

namespace defkeys
{

static constexpr char COMMENT[] = "comment", ID[] = "id";

static constexpr char CHR_WIDTH[] = "width", CHR_HEIGHT[] = "height",
											CHR_BPP[] = "bpp", CHR_PLANEOFFSET[] = "planeoffset",
											CHR_PIXELOFFSET[] = "pixeloffset",
											CHR_ROWOFFSET[] = "rowoffset";

static constexpr char PAL_ENTRY_DATASIZE[] = "entry_datasize",
											PAL_SUBPAL_LENGTH[] = "subpal_length",
											PAL_SUBPAL_COUNT[] = "subpal_count",
											PAL_SUBPAL_DATASIZE[] = "subpal_datasize";

static constexpr char COL_COLOR_PASSES[] = "color_passes",
											COL_RED_SHIFT[] = "red_shift",
											COL_RED_SIZE[] = "red_size",
											COL_GREEN_SHIFT[] = "green_shift",
											COL_GREEN_SIZE[] = "green_size",
											COL_BLUE_SHIFT[] = "blue_shift",
											COL_BLUE_SIZE[] = "blue_size",
											COL_BITDEPTH[] = "bitdepth", COL_REFTAB[] = "reftab",
											COL_BIG_ENDIAN[] = "big_endian";

} // namespace defkeys

gfxprofile validate_profile_block(defblock const &def_block)
{
	defblock::const_iterator mapiter;

	mapiter = def_block.find("id");
	if(mapiter == def_block.end()) {
		throw "Could not find profile ID";
	}
	string id{mapiter->second};

	mapiter = def_block.find("chrdef");
	if(mapiter == def_block.end()) {
		throw "Could not find chrdef in profile";
	}
	string chrdef{mapiter->second};

	mapiter = def_block.find("coldef");
	if(mapiter == def_block.end()) {
		throw "Could not find coldef in profile";
	}
	string coldef{mapiter->second};

	mapiter = def_block.find("paldef");
	if(mapiter == def_block.end()) {
		throw "Could not find paldef in profile";
	}
	string paldef{mapiter->second};

	return gfxprofile{id, chrdef, coldef, paldef};
}

chrdef validate_chrdef_block(defblock const &def_block)
{
	// instead of looping over the pairs, we'll go down each setting of the def
	// if the setting is present in the pair stack, validate the value
	// if it's missing and required, throw an error

	defblock::const_iterator mapiter;

	// SETTING: id
	// RULES: none
	mapiter = def_block.find(defkeys::ID);
	if(mapiter == def_block.end()) {
		throw "Could not find required key " + string(defkeys::ID);
	}
	string temp_id{mapiter->second};

	// SETTING: width
	// RULES: required, 8bit integer, positive
	mapiter = def_block.find(defkeys::CHR_WIDTH);
	if(mapiter == def_block.end()) {
		throw "Could not find required key " + string(defkeys::CHR_WIDTH);
	}
	auto temp_width{vd_int_pos<uint>(mapiter->second)};

	// SETTING: height
	// RULES: required, integer, non-negative, max value 255
	mapiter = def_block.find(defkeys::CHR_HEIGHT);
	if(mapiter == def_block.end()) {
		throw "Could not find required key " + string(defkeys::CHR_HEIGHT);
	}
	auto temp_height{vd_int_pos<uint>(mapiter->second)};

	// SETTING: bpp
	// RULES: required, integer, non-negative, max value 8
	mapiter = def_block.find(defkeys::CHR_BPP);
	if(mapiter == def_block.end()) {
		throw "Could not find required key " + string(defkeys::CHR_BPP);
	}
	auto temp_bpp{vd_int_pos<uint>(mapiter->second)};
	if(temp_bpp > 8) {
		throw "Value cannot be greater than 8";
	}

	// SETTING: planeoffset
	// RULES: required, array of integer, non-negative, max value 32bit
	// length = bpp
	mapiter = def_block.find(defkeys::CHR_PLANEOFFSET);
	if(mapiter == def_block.end()) {
		throw "Could not find required key " + string(defkeys::CHR_PLANEOFFSET);
	}
	auto temp_planeoffset{vd_int_array<uint>(mapiter->second)};
	if(temp_planeoffset.size() != temp_bpp) {
		throw "planeoffset must have number of entries equal to bpp";
	}

	// SETTING: pixeloffset
	// RULES: required, array of integers, non-negative, max value 32bit
	// length = width
	mapiter = def_block.find(defkeys::CHR_PIXELOFFSET);
	if(mapiter == def_block.end()) {
		throw "Could not find required key " + string(defkeys::CHR_PIXELOFFSET);
	}
	auto temp_pixeloffset{vd_int_array<uint>(mapiter->second)};
	if(temp_pixeloffset.size() != temp_width) {
		throw "pixeloffset must have number of entries equal to width";
	}

	// SETTING: rowoffset
	// RULES: required, array of integers, non-negative, max value 32bit
	// length = height
	mapiter = def_block.find(defkeys::CHR_ROWOFFSET);
	if(mapiter == def_block.end()) {
		throw "Could not find required key " + string(defkeys::CHR_ROWOFFSET);
	}
	auto temp_rowoffset{vd_int_array<uint>(mapiter->second)};
	if(temp_rowoffset.size() != temp_height) {
		throw "rowoffset must have number of entries equal to height";
	}
	return chrdef{temp_id,					temp_width,				temp_height,	 temp_bpp,
								temp_planeoffset, temp_pixeloffset, temp_rowoffset};
}

palette create_palette(std::string const &pal)
{

	string value{pal};
	// remove any spaces from the value before processing
	value.erase(std::remove(value.begin(), value.end(), ' '), value.end());

	std::stringstream ss(value);
	//, hex_conv;
	std::string this_value;
	u8 red{0}, green{0}, blue{0};
	palette out;
	try {
		while(std::getline(ss, this_value, ',')) {
			if(this_value[0] == '#')
				this_value.erase(this_value.begin(), this_value.begin() + 1);
			if(this_value.length() != 6)
				throw std::invalid_argument("Invalid hex color in reftab definition");

			red = std::stoi(this_value.substr(0, 2), nullptr, 16);
			green = std::stoi(this_value.substr(2, 2), nullptr, 16);
			blue = std::stoi(this_value.substr(4, 2), nullptr, 16);

			out.push_back(color{red, green, blue});
		}
	} catch(const std::invalid_argument &e) {
		throw std::invalid_argument("Invalid color value in array");
	}

	return out;
}

coldef validate_coldef_block(defblock const &def_block)
{

	defblock::const_iterator mapiter;

	// SETTING: id
	// RULES: none
	mapiter = def_block.find(defkeys::ID);
	if(mapiter == def_block.end()) {
		throw "Could not find required key " + string(defkeys::ID);
	}
	string temp_id{mapiter->second};

	// SETTING: is_big_endian
	// RULES: option, boolean
	mapiter = def_block.find(defkeys::COL_BIG_ENDIAN);
	bool temp_is_big_endian{false};
	if(mapiter != def_block.end()) {
		temp_is_big_endian = vd_bool(mapiter->second);
	}

	// SETTING: reftab
	mapiter = def_block.find(defkeys::COL_REFTAB);
	if(mapiter != def_block.end()) {

		// we have a ref table, parse it out
		return coldef(temp_id, create_palette(mapiter->second), temp_is_big_endian);
	} else {
		// SETTING: bitdepth
		// RULES: required, positive
		mapiter = def_block.find(defkeys::COL_BITDEPTH);
		if(mapiter == def_block.end()) {
			throw "Could not find required key " + string(defkeys::COL_BITDEPTH);
		}
		auto temp_bitdepth{vd_int_pos<uint>(mapiter->second)};

		// SETTING: color_passes
		// RULES: optional; defaults to 1; if set, must be > 0
		uint temp_color_passes{1};
		mapiter = def_block.find(defkeys::COL_COLOR_PASSES);
		if(mapiter != def_block.end()) {
			temp_color_passes = vd_int_pos<uint>(mapiter->second);
		}

		// SETTING: red_shift
		mapiter = def_block.find(defkeys::COL_RED_SHIFT);
		if(mapiter == def_block.end()) {
			throw "Could not find required key " + string(defkeys::COL_RED_SHIFT);
		}
		auto temp_red_shift{vd_int_array<int>(mapiter->second)};
		if(temp_red_shift.size() != temp_color_passes) {
			throw "red_shift must have number of entries equal to color_passes";
		}

		// SETTING: red_bitcount
		mapiter = def_block.find(defkeys::COL_RED_SIZE);
		if(mapiter == def_block.end()) {
			throw "Could not find required key " + string(defkeys::COL_RED_SIZE);
		}
		auto temp_red_size{vd_int_array<uint>(mapiter->second)};
		if(temp_red_size.size() != temp_color_passes) {
			throw "red_size must have number of entries equal to color_passes";
		}

		// SETTING: green_shift
		mapiter = def_block.find(defkeys::COL_GREEN_SHIFT);
		if(mapiter == def_block.end()) {
			throw "Could not find required key " + string(defkeys::COL_GREEN_SHIFT);
		}
		auto temp_green_shift{vd_int_array<int>(mapiter->second)};
		if(temp_green_shift.size() != temp_color_passes) {
			throw "green_shift must have number of entries equal to color_passes";
		}

		// SETTING: green_bitcount
		mapiter = def_block.find(defkeys::COL_GREEN_SIZE);
		if(mapiter == def_block.end()) {
			throw "Could not find required key " + string(defkeys::COL_GREEN_SIZE);
		}
		auto temp_green_size{vd_int_array<uint>(mapiter->second)};
		if(temp_green_size.size() != temp_color_passes) {
			throw "green_size must have number of entries equal to color_passes";
		}

		// SETTING: blue_shift
		mapiter = def_block.find(defkeys::COL_BLUE_SHIFT);
		if(mapiter == def_block.end()) {
			throw "Could not find required key " + string(defkeys::COL_BLUE_SHIFT);
		}
		auto temp_blue_shift{vd_int_array<int>(mapiter->second)};
		if(temp_blue_shift.size() != temp_color_passes) {
			throw "blue_shift must have number of entries equal to color_passes";
		}

		// SETTING: blue_bitcount
		mapiter = def_block.find(defkeys::COL_BLUE_SIZE);
		if(mapiter == def_block.end()) {
			throw "Could not find required key " + string(defkeys::COL_BLUE_SIZE);
		}
		auto temp_blue_size{vd_int_array<uint>(mapiter->second)};
		if(temp_blue_size.size() != temp_color_passes) {
			throw "blue_size must have number of entries equal to color_passes";
		}

		vector<rgb_layout> passes;
		passes.reserve(temp_color_passes);

		for(int pass_iter = 0; pass_iter < temp_color_passes; ++pass_iter) {
			passes.push_back(
					rgb_layout{{temp_red_shift[pass_iter], temp_red_size[pass_iter]},
										 {temp_green_shift[pass_iter], temp_green_size[pass_iter]},
										 {temp_blue_shift[pass_iter], temp_blue_size[pass_iter]}});
		}

		return coldef{temp_id, temp_bitdepth, passes, temp_is_big_endian};
	}
}

paldef validate_paldef_block(defblock const &def_block)
{
	defblock::const_iterator mapiter;

	// SETTING: id
	// RULES: required
	mapiter = def_block.find(defkeys::ID);
	if(mapiter == def_block.end()) {
		throw "Could not find required key " + string(defkeys::ID);
	}
	string temp_id{mapiter->second};

	// SETTING: entry_datasize
	// RULES: required
	mapiter = def_block.find(defkeys::PAL_ENTRY_DATASIZE);
	if(mapiter == def_block.end()) {
		throw "Could not find required key " + string(defkeys::PAL_ENTRY_DATASIZE);
	}
	auto temp_entry_datasize{vd_int_pos<uint>(mapiter->second)};

	// SETTING: subpal_length
	// RULES: required
	mapiter = def_block.find(defkeys::PAL_SUBPAL_LENGTH);
	if(mapiter == def_block.end()) {
		throw "Could not find required key " + string(defkeys::PAL_SUBPAL_LENGTH);
	}
	auto temp_subpal_length{vd_int_pos<uint>(mapiter->second)};

	// SETTING: subpal_count
	// RULES: required
	mapiter = def_block.find(defkeys::PAL_SUBPAL_COUNT);
	if(mapiter == def_block.end()) {
		throw "Could not find required key " + string(defkeys::PAL_SUBPAL_COUNT);
	}
	auto temp_subpal_count{vd_int_pos<uint>(mapiter->second)};

	// SETTING: subpal_datasize
	// RULES: optional
	mapiter = def_block.find(defkeys::PAL_SUBPAL_DATASIZE);
	std::optional<uint> temp_subpal_datasize{std::nullopt};
	if(mapiter != def_block.end()) {
		temp_subpal_datasize = vd_int_pos<uint>(mapiter->second);
	}

	return paldef{temp_id, temp_entry_datasize, temp_subpal_length,
								temp_subpal_count, temp_subpal_datasize};
}

std::tuple<map<string const, chrdef const>, map<string const, coldef const>,
					 map<string const, paldef const>, map<string const, gfxprofile const>>
load_gfxdefs(string const &def_file)
{
	std::multimap<string const, defblock const> blocks{load_defblocks(def_file)};
	// std::multimap<string const, defblock const>::iterator block_iter;
	auto block_iter = blocks.equal_range("chrdef");
	map<string const, chrdef const> chrdefs;

	for(auto it = block_iter.first; it != block_iter.second; ++it) {
		chrdef temp_def = validate_chrdef_block(it->second);
		chrdefs.insert({temp_def.get_id(), std::move(temp_def)});
	}
	// add library builtin def
	chrdefs.insert({defs::basic_8x8_1bpp.get_id(), defs::basic_8x8_1bpp});

	map<string const, coldef const> coldefs;
	// block_iter = blocks.find("coldef");
	block_iter = blocks.equal_range("coldef");
	for(auto it = block_iter.first; it != block_iter.second; ++it) {
		coldef temp_def = validate_coldef_block(it->second);
		coldefs.insert({temp_def.get_id(), std::move(temp_def)});
		// block_iter++;
	}

	coldefs.insert({defs::basic_8bit_random.get_id(), defs::basic_8bit_random});

	map<string const, paldef const> paldefs;
	// block_iter = blocks.find("paldef");
	block_iter = blocks.equal_range("paldef");
	for(auto it = block_iter.first; it != block_iter.second; ++it) {
		paldef temp_def = validate_paldef_block(it->second);
		paldefs.insert({temp_def.get_id(), std::move(temp_def)});
		// block_iter++;
	}

	paldefs.insert({defs::basic_256color.get_id(), defs::basic_256color});

	map<string const, gfxprofile const> profiles;
	// block_iter = blocks.find("profile");
	block_iter = blocks.equal_range("profile");
	for(auto it = block_iter.first; it != block_iter.second; ++it) {
		gfxprofile temp_def{validate_profile_block(it->second)};
		profiles.insert({temp_def.get_id(), temp_def});
		//++block_iter;
	}

	return std::make_tuple(chrdefs, coldefs, paldefs, profiles);
}
