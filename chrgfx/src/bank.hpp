#ifndef CHRGFX__BANK_H
#define CHRGFX__BANK_H

#include "gfxdef.hpp"
#include "types.hpp"
#include <png++/png.hpp>
#include <vector>

using std::vector;

namespace chrgfx
{
class bank : public vector<u8 *>
{
public:
	bank(chr_def const &chrdef);

	~bank();

	chr_def get_chrdef();

private:
	chr_def chrdef;
};

} // namespace chrgfx

#endif