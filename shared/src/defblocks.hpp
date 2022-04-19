#ifndef __MOTOI__DEFBLOCKS_H
#define __MOTOI__DEFBLOCKS_H

#include <istream>
#include <map>
#include <stdexcept>
#include <string>

class defblock_key_error : public std::runtime_error
{
public:
	defblock_key_error(std::string const & what, std::string const & key,
										 std::string const & block_id = nullptr);

	std::string key() const;
	std::string block() const;

private:
	std::string const m_key;
	std::string const m_block_id;
	std::string const m_what;
};

class defblock_value_error : public defblock_key_error
{
public:
	defblock_value_error(std::string const & what, std::string const & key,
											 std::string const & value,
											 std::string const & block_id = nullptr);

	std::string value() const;

private:
	std::string const m_value;
};

typedef std::map<std::string const, std::string const> defblock;

std::multimap<std::string const, defblock const>
load_defblocks(std::istream & in);

#endif
