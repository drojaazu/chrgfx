#include "import_defs.hpp"
#include "builtin_defs.hpp"
#include "fstreams.hpp"
#include "parsing.hpp"
#include <cerrno>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;
using namespace chrgfx;
using namespace png;

namespace defkeys
{

char const * COMMENT = "comment";
char const * ID = "id";

char const * CHR_WIDTH = "width";
char const * CHR_HEIGHT = "height";
char const * CHR_BPP = "bpp";
char const * CHR_PLANEOFFSET = "planeoffset";
char const * CHR_PIXELOFFSET = "pixeloffset";
char const * CHR_ROWOFFSET = "rowoffset";

char const * PAL_ENTRY_DATASIZE = "entry_datasize";
char const * PAL_PAL_LENGTH = "pal_length";
char const * PAL_PAL_DATASIZE = "pal_datasize";

char const * COL_COLOR_PASSES = "color_passes";
char const * COL_RED_SHIFT = "red_shift";
char const * COL_RED_SIZE = "red_size";
char const * COL_GREEN_SHIFT = "green_shift";
char const * COL_GREEN_SIZE = "green_size";
char const * COL_BLUE_SHIFT = "blue_shift";
char const * COL_BLUE_SIZE = "blue_size";
char const * COL_BITDEPTH = "bitdepth";
char const * COL_REFTAB = "reftab";
char const * COL_BIG_ENDIAN = "big_endian";

// prefixes for profile definitions
char const * PRF_CHRDEF = "chrdef";
char const * PRF_COLDEF = "coldef";
char const * PRF_PALDEF = "paldef";

} // namespace defkeys

char const * ERR_KEY_NOT_FOUND = "Could not find required key";

palette parse_palette(string const & pal)
{
	string value {pal};
	// remove any spaces from the value before processing
	value.erase(remove(value.begin(), value.end(), ' '), value.end());

	stringstream ss(value);
	string this_value;
	u8 red {0}, green {0}, blue {0};
	palette out;
	while (getline(ss, this_value, ','))
	{
		if (this_value[0] == '#')
		{
			this_value.erase(this_value.begin(), this_value.begin() + 1);
		}
		if (this_value.length() != 6)
		{
			throw invalid_argument("Invalid HTML formatted color: '" + this_value + "'");
		}
		try
		{
			red = sto<u8>(this_value.substr(0, 2), 16);
			green = sto<u8>(this_value.substr(2, 2), 16);
			blue = sto<u8>(this_value.substr(4, 2), 16);
		}
		catch (invalid_argument const &)
		{
			throw invalid_argument("Invalid HTML formatted color: '" + this_value + "'");
		}
		out.push_back(color {red, green, blue});
	}

	return out;
}

gfxprofile validate_profile_block(defblock const & def_block)
{
	defblock::const_iterator mapiter;

	mapiter = def_block.find(defkeys::ID);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error(ERR_KEY_NOT_FOUND, defkeys::ID);
	}
	string temp_id {mapiter->second};

	mapiter = def_block.find(defkeys::COMMENT);
	string temp_comment;
	if (mapiter != def_block.end())
	{
		temp_comment = mapiter->second;
	}

	mapiter = def_block.find(defkeys::PRF_CHRDEF);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error("Could not find tile definition in gfxdef profile", defkeys::PRF_CHRDEF, temp_id);
	}
	string temp_chrdef {mapiter->second};

	mapiter = def_block.find(defkeys::PRF_COLDEF);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error("Could not find color defition in gfxdef profile", defkeys::PRF_COLDEF, temp_id);
	}
	string temp_coldef {mapiter->second};

	mapiter = def_block.find(defkeys::PRF_PALDEF);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error("Could not find palette definition in gfxdef profile", defkeys::PRF_PALDEF, temp_id);
	}
	string temp_paldef {mapiter->second};

	return gfxprofile {temp_id, temp_chrdef, temp_coldef, temp_paldef, temp_comment};
}

// instead of looping over the pairs, we'll go down each setting of the def
// if the setting is present in the pair stack, validate the value
// if it's missing and required, throw an error

chrdef validate_chrdef_block(defblock const & def_block)
{
	defblock::const_iterator mapiter;

	// SETTING: id
	// RULES: required
	mapiter = def_block.find(defkeys::ID);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error(ERR_KEY_NOT_FOUND, defkeys::ID);
	}
	string temp_id {mapiter->second};

	mapiter = def_block.find(defkeys::COMMENT);
	string temp_comment;
	if (mapiter != def_block.end())
	{
		temp_comment = mapiter->second;
	}

	// SETTING: width
	// RULES: required, positive integer
	mapiter = def_block.find(defkeys::CHR_WIDTH);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error(ERR_KEY_NOT_FOUND, defkeys::CHR_WIDTH, temp_id);
	}
	auto temp_width {sto_pos<ushort>(mapiter->second)};

	// SETTING: height
	// RULES: required, integer, non-negative, max value 255
	mapiter = def_block.find(defkeys::CHR_HEIGHT);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error(ERR_KEY_NOT_FOUND, defkeys::CHR_HEIGHT, temp_id);
	}
	auto temp_height {sto_pos<ushort>(mapiter->second)};

	// SETTING: bpp
	// RULES: required, integer, non-negative, max value 8
	mapiter = def_block.find(defkeys::CHR_BPP);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error(ERR_KEY_NOT_FOUND, defkeys::CHR_BPP, temp_id);
	}
	auto temp_bpp {sto_pos<ushort>(mapiter->second)};
	if (temp_bpp > 8)
	{
		throw defblock_value_error("Value cannot be greater than 8", to_string(temp_bpp), defkeys::CHR_BPP, temp_id);
	}

	// SETTING: planeoffset
	// RULES: required, array of integer, non-negative, max value 32bit
	// length = bpp
	mapiter = def_block.find(defkeys::CHR_PLANEOFFSET);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error(ERR_KEY_NOT_FOUND, defkeys::CHR_PLANEOFFSET, temp_id);
	}
	auto temp_planeoffset {stovec<ushort>(mapiter->second)};
	if (temp_planeoffset.size() != temp_bpp)
	{
		throw defblock_value_error("planeoffset must have number of entries equal to value of bpp",
			mapiter->second,
			defkeys::CHR_PLANEOFFSET,
			temp_id);
	}

	// SETTING: pixeloffset
	// RULES: required, array of integers, non-negative, max value 32bit
	// length = width
	mapiter = def_block.find(defkeys::CHR_PIXELOFFSET);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error(ERR_KEY_NOT_FOUND, defkeys::CHR_PIXELOFFSET, temp_id);
	}
	auto temp_pixeloffset {stovec<ushort>(mapiter->second)};
	if (temp_pixeloffset.size() != temp_width)
	{
		throw defblock_value_error("pixeloffset must have number of entries equal to value of width",
			mapiter->second,
			defkeys::CHR_PIXELOFFSET,
			temp_id);
	}

	// SETTING: rowoffset
	// RULES: required, array of integers, non-negative, max value 32bit
	// length = height
	mapiter = def_block.find(defkeys::CHR_ROWOFFSET);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error(ERR_KEY_NOT_FOUND, defkeys::CHR_ROWOFFSET, temp_id);
	}
	auto temp_rowoffset {stovec<ushort>(mapiter->second)};
	if (temp_rowoffset.size() != temp_height)
	{
		throw defblock_value_error("rowoffset must have number of entries equal to value of height",
			mapiter->second,
			defkeys::CHR_ROWOFFSET,
			temp_id);
	}

	return chrdef {
		temp_id, temp_width, temp_height, temp_bpp, temp_planeoffset, temp_pixeloffset, temp_rowoffset, temp_comment};
}

refcoldef validate_refcoldef_block(defblock const & def_block)
{
	// SETTING: id
	// RULES: none
	auto mapiter = def_block.find(defkeys::ID);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error(ERR_KEY_NOT_FOUND, defkeys::ID);
	}
	string temp_id {mapiter->second};

	mapiter = def_block.find(defkeys::COMMENT);
	string temp_comment;
	if (mapiter != def_block.end())
	{
		temp_comment = mapiter->second;
	}

	// SETTING: is_big_endian
	// RULES: option, boolean
	mapiter = def_block.find(defkeys::COL_BIG_ENDIAN);
	bool temp_is_big_endian {false};
	if (mapiter != def_block.end())
	{
		temp_is_big_endian = stob(mapiter->second);
	}

	// SETTING: reftab
	// RULES: required, array of html formatted colors
	mapiter = def_block.find(defkeys::COL_REFTAB);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error(ERR_KEY_NOT_FOUND, defkeys::COL_REFTAB, temp_id);
	}

	png::palette temp_pal;
	try
	{
		temp_pal = parse_palette(mapiter->second);
	}
	catch (invalid_argument const & e)
	{
		throw defblock_value_error(e.what(), mapiter->second, defkeys::COL_REFTAB, temp_id);
	}
	return {temp_id, temp_pal, temp_is_big_endian, temp_comment};
}

rgbcoldef validate_rgbcoldef_block(defblock const & def_block)
{
	// SETTING: id
	// RULES: none
	auto mapiter = def_block.find(defkeys::ID);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error(ERR_KEY_NOT_FOUND, defkeys::ID);
	}
	string temp_id {mapiter->second};

	mapiter = def_block.find(defkeys::COMMENT);
	string temp_comment;
	if (mapiter != def_block.end())
	{
		temp_comment = mapiter->second;
	}

	// SETTING: is_big_endian
	// RULES: option, boolean
	mapiter = def_block.find(defkeys::COL_BIG_ENDIAN);
	bool temp_is_big_endian {false};
	if (mapiter != def_block.end())
	{
		temp_is_big_endian = stob(mapiter->second);
	}

	// SETTING: bitdepth
	// RULES: required, positive
	mapiter = def_block.find(defkeys::COL_BITDEPTH);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error(ERR_KEY_NOT_FOUND, defkeys::COL_BITDEPTH, temp_id);
	}
	auto temp_bitdepth {sto_pos<ushort>(mapiter->second)};

	// SETTING: color_passes
	// RULES: optional; defaults to 1; if set, must be > 0
	uint temp_color_passes {1};
	mapiter = def_block.find(defkeys::COL_COLOR_PASSES);
	if (mapiter != def_block.end())
	{
		temp_color_passes = sto_pos<ushort>(mapiter->second);
	}

	// SETTING: red_shift
	mapiter = def_block.find(defkeys::COL_RED_SHIFT);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error(ERR_KEY_NOT_FOUND, defkeys::COL_RED_SHIFT, temp_id);
	}
	auto temp_red_shift {stovec<int>(mapiter->second)};
	if (temp_red_shift.size() != temp_color_passes)
	{
		throw "red_shift must have number of entries equal to color_passes";
	}

	// SETTING: red_bitcount
	mapiter = def_block.find(defkeys::COL_RED_SIZE);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error(ERR_KEY_NOT_FOUND, defkeys::COL_RED_SIZE, temp_id);
	}
	auto temp_red_size {stovec<ushort>(mapiter->second)};
	if (temp_red_size.size() != temp_color_passes)
	{
		throw "red_size must have number of entries equal to color_passes";
	}

	// SETTING: green_shift
	mapiter = def_block.find(defkeys::COL_GREEN_SHIFT);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error(ERR_KEY_NOT_FOUND, defkeys::COL_GREEN_SHIFT, temp_id);
	}
	auto temp_green_shift {stovec<int>(mapiter->second)};
	if (temp_green_shift.size() != temp_color_passes)
	{
		throw "green_shift must have number of entries equal to color_passes";
	}

	// SETTING: green_bitcount
	mapiter = def_block.find(defkeys::COL_GREEN_SIZE);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error(ERR_KEY_NOT_FOUND, defkeys::COL_GREEN_SIZE, temp_id);
	}
	auto temp_green_size {stovec<ushort>(mapiter->second)};
	if (temp_green_size.size() != temp_color_passes)
	{
		throw "green_size must have number of entries equal to color_passes";
	}

	// SETTING: blue_shift
	mapiter = def_block.find(defkeys::COL_BLUE_SHIFT);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error(ERR_KEY_NOT_FOUND, defkeys::COL_BLUE_SHIFT, temp_id);
	}
	auto temp_blue_shift {stovec<int>(mapiter->second)};
	if (temp_blue_shift.size() != temp_color_passes)
	{
		throw "blue_shift must have number of entries equal to color_passes";
	}

	// SETTING: blue_bitcount
	mapiter = def_block.find(defkeys::COL_BLUE_SIZE);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error(ERR_KEY_NOT_FOUND, defkeys::COL_BLUE_SIZE, temp_id);
	}
	auto temp_blue_size {stovec<ushort>(mapiter->second)};
	if (temp_blue_size.size() != temp_color_passes)
	{
		throw "blue_size must have number of entries equal to color_passes";
	}

	vector<rgb_layout> passes;
	passes.reserve(temp_color_passes);

	for (int pass_iter = 0; pass_iter < temp_color_passes; ++pass_iter)
	{
		passes.push_back(rgb_layout {{temp_red_shift[pass_iter], temp_red_size[pass_iter]},
			{temp_green_shift[pass_iter], temp_green_size[pass_iter]},
			{temp_blue_shift[pass_iter], temp_blue_size[pass_iter]}});
	}

	return rgbcoldef {temp_id, temp_bitdepth, passes, temp_is_big_endian, temp_comment};
}

paldef validate_paldef_block(defblock const & def_block)
{
	defblock::const_iterator mapiter;

	// SETTING: id
	// RULES: required
	mapiter = def_block.find(defkeys::ID);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error(ERR_KEY_NOT_FOUND, defkeys::ID);
	}
	string temp_id {mapiter->second};

	mapiter = def_block.find(defkeys::COMMENT);
	string temp_comment;
	if (mapiter != def_block.end())
	{
		temp_comment = mapiter->second;
	}

	// SETTING: entry_datasize
	// RULES: required
	mapiter = def_block.find(defkeys::PAL_ENTRY_DATASIZE);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error(ERR_KEY_NOT_FOUND, defkeys::PAL_ENTRY_DATASIZE, temp_id);
	}
	auto temp_entry_datasize {sto<ushort>(mapiter->second)};

	// SETTING: subpal_length
	// RULES: required
	mapiter = def_block.find(defkeys::PAL_PAL_LENGTH);
	if (mapiter == def_block.end())
	{
		throw defblock_key_error(ERR_KEY_NOT_FOUND, defkeys::PAL_PAL_LENGTH, temp_id);
	}
	auto temp_pal_length {sto<ushort>(mapiter->second)};

	// SETTING: pal_datasize
	// RULES: optional
	mapiter = def_block.find(defkeys::PAL_PAL_DATASIZE);
	optional<uint> temp_pal_datasize {nullopt};
	if (mapiter != def_block.end())
	{
		temp_pal_datasize = sto<ushort>(mapiter->second);
	}

	return paldef {temp_id, temp_entry_datasize, temp_pal_length, temp_pal_datasize, temp_comment};
}

def_collection load_gfxdefs(string const & def_file)
{
	auto def_stream = ifstream_checked(def_file);
	multimap<string const, defblock const> blocks {load_defblocks(def_stream)};
	// multimap<string const, defblock const>::iterator block_iter;
	auto block_iter = blocks.equal_range("chrdef");
	map<string const, chrdef const> chrdefs;

	for (auto it = block_iter.first; it != block_iter.second; ++it)
	{
		chrdef temp_def = validate_chrdef_block(it->second);
		chrdefs.insert({temp_def.id(), std::move(temp_def)});
	}
	// add library builtin def
	chrdefs.insert({string(gfxdefs::basic_8x8_1bpp.id()), gfxdefs::basic_8x8_1bpp});

	map<string const, rgbcoldef const> rgbcoldefs;
	// block_iter = blocks.find("coldef");
	block_iter = blocks.equal_range("rgbcoldef");
	for (auto it = block_iter.first; it != block_iter.second; ++it)
	{
		rgbcoldef temp_def = validate_rgbcoldef_block(it->second);
		rgbcoldefs.insert({temp_def.id(), std::move(temp_def)});
		// block_iter++;
	}

	map<string const, refcoldef const> refcoldefs;
	// block_iter = blocks.find("coldef");
	block_iter = blocks.equal_range("refcoldef");
	for (auto it = block_iter.first; it != block_iter.second; ++it)
	{
		refcoldef temp_def = validate_refcoldef_block(it->second);
		refcoldefs.insert({temp_def.id(), std::move(temp_def)});
		// block_iter++;
	}

	refcoldefs.insert({string(gfxdefs::basic_8bit_random.id()), gfxdefs::basic_8bit_random});

	map<string const, paldef const> paldefs;
	// block_iter = blocks.find("paldef");
	block_iter = blocks.equal_range("paldef");
	for (auto it = block_iter.first; it != block_iter.second; ++it)
	{
		paldef temp_def = validate_paldef_block(it->second);
		paldefs.insert({temp_def.id(), std::move(temp_def)});
		// block_iter++;
	}

	paldefs.insert({string(gfxdefs::basic_256color.id()), gfxdefs::basic_256color});

	map<string const, gfxprofile const> profiles;
	// block_iter = blocks.find("profile");
	block_iter = blocks.equal_range("profile");
	for (auto it = block_iter.first; it != block_iter.second; ++it)
	{
		gfxprofile temp_def {validate_profile_block(it->second)};
		profiles.insert({temp_def.id(), temp_def});
		//++block_iter;
	}

	return {chrdefs, rgbcoldefs, refcoldefs, paldefs, profiles};
}
