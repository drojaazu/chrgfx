#include "import_defs.hpp"
#include "builtin_defs.hpp"
#include "simdjson.h"

using namespace simdjson;

def_collection load_gfxdefs(string const & def_file)
{
	ondemand::parser parser;
	auto json = padded_string::load(def_file);
	ondemand::document doc = parser.iterate(json); // position a pointer at the beginning of the JSON data
}