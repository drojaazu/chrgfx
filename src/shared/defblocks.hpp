#ifndef __MOTOI__DEFBLOCKS_H
#define __MOTOI__DEFBLOCKS_H

#include <istream>
#include <map>
#include <stdexcept>
#include <string>

class defblock_key_error : public std::runtime_error
{
public:
	defblock_key_error(std::string const & what, std::string const & key);

	[[nodiscard]] std::string key() const;

private:
	std::string const m_key;
};

class defblock_value_error : public std::runtime_error
{
public:
	defblock_value_error(std::string const & what, std::string const & key, std::string const & value);

	[[nodiscard]] std::string key() const;
	[[nodiscard]] std::string value() const;

private:
	std::string const m_key;
	std::string const m_value;
};

using defblock = std::map<const std::string, const std::string>;

std::multimap<std::string const, defblock const> load_defblocks(std::istream & in);

#endif
