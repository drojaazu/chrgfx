#ifndef LIBCHRGFX_BANK_DEFS_H
#define LIBCHRGFX_BANK_DEFS_H

#include "gfxdef.hpp"
#include "types.hpp"
#include <vector>

namespace chrgfx
{

class bank : public std::vector<u8 *>
{
public:
	bank(chr_def const &chrdef);
	~bank();

	chr_def get_chrdef();
};

} // namespace chrgfx

#endif
