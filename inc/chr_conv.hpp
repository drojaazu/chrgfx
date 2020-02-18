#ifndef CHR_CONV_H
#define CHR_CONV_H
#include "types.hpp"

namespace chrgfx
{
defchr get_defchr(chr_def &chrdef, rawchr data);
rawchr get_rawchr(chr_def &chrdef, defchr data);

static const std::map<const std::string, chr_cv> chr_converters{
		{std::string("get_chr"), get_rawchr}};
} // namespace chrgfx

#endif