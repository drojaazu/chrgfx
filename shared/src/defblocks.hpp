#ifndef DEFBLOCKS_H
#define DEFBLOCKS_H

#include <map>
#include <string>

typedef std::map<std::string const, std::string const> defblock;

std::string ltrim(std::string const & s);
std::string rtrim(std::string const & s);
std::string trim(std::string const & s);

std::multimap<std::string const, defblock const>
load_defblocks(std::string const & file);

#endif
