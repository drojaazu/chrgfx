#ifndef DEFBLOCKS_H
#define DEFBLOCKS_H

#include <map>
#include <string>
#include <vector>

typedef std::map<std::string const, std::string const> defblock;

std::map<std::string const, defblock const>
load_defblocks(std::string const &file);

#endif
