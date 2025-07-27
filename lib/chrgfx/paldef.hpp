/**
 * @file paldef.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @copyright ©2022 Motoi Productions / Released under MIT License
 * @brief Palette format definition
 */

#ifndef __CHRGFX__PALDEF_HPP
#define __CHRGFX__PALDEF_HPP

#include "gfxdef.hpp"
#include "types.hpp"
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
	paldef(std::string const & id, uint const entry_datasize, uint const length, std::string const & description = "");

	paldef(std::string const & id,
		uint const entry_datasize,
		uint const length,
		std::optional<uint const> const datasize,
		std::string const & description = "");

	/**
	 * @return number of entries in the palette
	 */
	[[nodiscard]] uint length() const;

	/**
	 * @return data size of a single entry *in bits*
	 */
	[[nodiscard]] uint entry_datasize() const;

	/**
	 * @return data size of a single entry *in bytes*
	 */
	[[nodiscard]] uint entry_datasize_bytes() const;

	/**
	 * @return data size of the whole palette *in bits*
	 */
	[[nodiscard]] uint datasize() const;

	/**
	 * @return data size of the whole palette *in bytes*
	 */
	[[nodiscard]] uint datasize_bytes() const;

protected:
	uint m_length;
	uint m_entry_datasize;
	uint m_entry_datasize_bytes;
	uint m_datasize;
	uint m_datasize_bytes;
};

} // namespace chrgfx

#endif
