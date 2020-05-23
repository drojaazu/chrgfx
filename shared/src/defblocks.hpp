#ifndef DEFBLOCKS_H
#define DEFBLOCKS_H

#include <bits/stdc++.h>
#include <cerrno>
#include <map>
#include <string>
#include <vector>

using std::map;
using std::pair;
using std::string;
using std::vector;

typedef map<string const, string const> defblock;

string ltrim(string const &s);
string rtrim(string const &s);
string trim(string const &s);

std::multimap<string const, defblock const> load_defblocks(string const &file);

#endif
