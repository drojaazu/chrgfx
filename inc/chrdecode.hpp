#include "chrdef.hpp"

namespace chrgfx
{
class chr_decode
{
 public:
	chr_decode(const chr_def chrdef);
	const u8 *get_chr(const u8 *data);
	const chr_def *get_def();
	~chr_decode(){};

 private:
	chr_def chrdef;
};

}	// namespace chrgfx