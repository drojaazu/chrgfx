#ifndef __MOTOI__DEFBLOCKS_H
#define __MOTOI__DEFBLOCKS_H

#include <istream>
#include <map>
#include <stdexcept>
#include <string>

class defblock_key_error : public std::runtime_error
{
public:
	defblock_key_error(char const * what, char const * key,
										 char const * block_id = nullptr);

	char const * key() const;
	char const * block() const;

private:
	char const * m_key;
	char const * m_block_id;
	char const * m_what;
};

class defblock_value_error : public defblock_key_error
{
public:
	defblock_value_error(char const * what, char const * key, char const * value,
											 char const * block_id = nullptr);

	char const * value() const;

private:
	char const * m_value;
};

typedef std::map<std::string const, std::string const> defblock;

std::multimap<std::string const, defblock const>
load_defblocks(std::istream & in);

#endif
