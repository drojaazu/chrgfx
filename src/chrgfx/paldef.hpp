/**
 * @file paldef.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @copyright ©2022 Motoi Productions / Released under MIT License
 * @brief Palette format definition
 */

#ifndef __CHRGFX__PALDEF_HPP
#define __CHRGFX__PALDEF_HPP

#include "gfxdef.hpp"
#include <optional>
#include <string>

namespace chrgfx
{

/**
 * @brief Palette encoding
 */
class paldef : public gfxdef
{
public:
	paldef(std::string const & id,
		uint const entry_datasize,
		uint const pal_length,
		std::optional<uint const> const pal_datasize = std::nullopt,
		std::string const & description = "");

	/**
	 * @return number of entries in the palette
	 */
	[[nodiscard]] uint pal_length() const;

	/**
	 * @return data size of a single entry *in bits*
	 */
	[[nodiscard]] uint entry_datasize() const;

	/**
	 * @return data size of the whole palette *in bits*
	 */
	[[nodiscard]] uint datasize() const;

protected:
	uint m_pal_length;
	uint m_entry_datasize;
	uint m_subpal_datasize;
};

} // namespace chrgfx

#endif
