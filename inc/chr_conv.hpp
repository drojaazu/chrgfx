#ifndef CHR_CONV_H
#define CHR_CONV_H
#include "types.hpp"

namespace chrgfx
{
bptr to_chr(chr_def &chrdef, bptr data);
u8 *get_chr(chr_def &chrdef, u8 *data);

static const std::map<const std::string, chr_cv> chr_converters{
		{std::string("get_chr"), get_chr}};
} // namespace chrgfx

#endif