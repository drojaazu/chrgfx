#ifndef SNK_NEOGEO_GFX
#define SNK_NEOGEO_GFX

#include "utils.hpp"

using namespace png;

namespace chrgfx
{
class snk_neogeo_cx : public chr_xform
{
 private:
	static const chr_traits traits;
	static const chr* get_chr_neogeo(const u8* data);

 public:
	const chr_traits* get_traits();
	const chr* get_chr(const u8* data);
};

class snk_neogeocd_cx : public snk_neogeo_cx
{
 private:
	static const chr* get_chr_neogeocd(const u8* data);

 public:
	const chr* get_chr(const u8* data);
};
}	// namespace chrgfx
#endif