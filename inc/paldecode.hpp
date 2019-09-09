#ifndef PALDECODE_H
#define PALDECODE_H
#include "paldef.hpp"

namespace chrgfx
{
class pal_decode
{
 public:
	// ctor/dtor
	pal_decode(pal_def paldef);
	~pal_decode(){};

	const palette *const get_pal(const u8 *data);
	const color get_color(const u32 data);

	// getters
	const pal_def &get_paldef() const { return this->paldef; };

 private:
	bool use_syspal = false;
	pal_def paldef;
};
}	// namespace chrgfx
#endif