#ifndef CHRGFX__CONV_CHR_H
#define CHRGFX__CONV_CHR_H

#include "buffer.hpp"
#include "chrdef.hpp"
#include "types.hpp"
//#include <iomanip>
#include <map>
#include <string>

namespace chrgfx
{
	/**
	 * Represents a function to convert a tile to a given encoding
	 */
	// using ChrConverterT = buffer (*)(chrdef const &, buffer const &);

	/**
	 * Convert a tile to the specified encoding
	 */
	void toFormattedChr(chrdef const & to_def, u8 const * in_data, u8 * out_data);

	/**
	 * Convert a tile from the specified encoding
	 */
	void toBasicChr(chrdef const & from_def, u8 const * in_data, u8 * out_data);

} // namespace chrgfx

#endif