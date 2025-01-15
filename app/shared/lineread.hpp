/**
 * @file lineread.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @brief Simple class for reading lines from a file and maintaining the state
 *        of the read pointer
 * @copyright ©2024 Motoi Productions / Released under MIT License
 *
 * Updates:
 * 20241201 Initial
 */

#ifndef __MOTOI__LINEREAD_HPP
#define __MOTOI__LINEREAD_HPP

#include "fstreams.hpp"
#include <string>

namespace motoi
{

template <typename StringT = char>
class linereader
{

protected:
	std::basic_string<StringT> m_filepath;
	std::basic_ifstream<StringT> m_ifstream;
	std::basic_string<StringT> m_line;
	size_t m_line_num {0};

public:
	linereader() = delete;
	linereader(std::string const filepath) :
			m_filepath {filepath},
			m_ifstream {motoi::ifstream_checked(m_filepath)} {};

	/**
	 * @return size_t The current line number.
	 */
	size_t line_number()
	{
		return m_line_num;
	}

	/**
	 * @return std::string_view The current line.
	 */
	std::basic_string_view<StringT> line()
	{
		return m_line;
	}

	/**
	 * @brief Moves the read head to the next line.
	 *
	 * @return false EOF or other stream error
	 */
	bool next()
	{
		if (m_ifstream.good())
		{
			getline(m_ifstream, m_line);
			++m_line_num;
			return true;
		}
		return false;
	}

	/**
	 * @return std::string_view Path of the currently open file
	 */
	std::basic_string_view<StringT> filepath()
	{
		return m_filepath;
	}

	/**
	 * @brief Reset line read head back to the start of the file
	 *
	 */
	void reset()
	{
		m_ifstream.clear();
		m_ifstream.seekg(0, std::ios::beg);
		m_line_num = 0;
	}
};

} // namespace motoi
#endif