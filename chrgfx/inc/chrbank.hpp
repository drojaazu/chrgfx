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
 * Container for tiles with definition
 */
class chrbank : public vector<u8 *>
{
public:
	chrbank(class chrdef const &chrdef);

	~chrbank();

	chrdef get_chrdef();

private:
	chrdef def;
};

} // namespace chrgfx

#endif