#ifndef CHRGFX__BANK_H
#define CHRGFX__BANK_H

#include "gfxdef.hpp"
#include "types.hpp"
#include <png++/png.hpp>
#include <vector>

using std::vector;

namespace chrgfx
{

/**
 *
 */
class chrbank : public vector<uptr<u8>>
{
public:
	chrbank(chrdef chrdef);

	chrdef get_chrdef() const;

private:
	chrdef def;
};

} // namespace chrgfx

#endif