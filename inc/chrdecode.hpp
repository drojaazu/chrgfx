#ifndef CHRDECODE_H
#define CHRDECODE_H
#include "gfx_types.hpp"

namespace chrgfx
{
const u8* get_chr(const chr_def* chrdef, const u8* data);

}	// namespace chrgfx

#endif