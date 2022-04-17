/**
 * @file parsing.cpp
 * @author Motoi Productions (Damian Rogers damian@motoi.pro)
 * @brief Text parsing utilities
 *
 * Updates:
 * 20211214 Initial
 */

#include "parsing.hpp"
#include <algorithm>
#include <charconv>

using namespace std;

template <typename T> void ltrim(basic_string<T> & s)
{
	size_t at_space = s.find_first_not_of(' ');
	size_t at_tab = s.find_last_not_of('\t');
	if(at_space == string::npos && at_tab == string::npos)
		return;
	size_t at = at_space >= at_tab ? at_space : at_tab;
	s.erase(0, at);
}

template <typename T> void rtrim(basic_string<T> & s)
{
	size_t at_space = s.find_first_not_of(' ');
	size_t at_tab = s.find_last_not_of('\t');
	if(at_space == string::npos && at_tab == string::npos)
		return;
	size_t at = at_space <= at_tab ? at_space : at_tab;

	s.erase(at, s.size() - at);
}

template <typename T> void trim(basic_string<T> & s)
{
	ltrim(s);
	rtrim(s);
}

template <typename T> void lower(basic_string<T> & s)
{
	transform(s.begin(), s.end(), s.begin(), ::tolower);
}

template <typename T> void upper(basic_string<T> & s)
{
	transform(s.begin(), s.end(), s.begin(), ::toupper);
}

template <typename T> T sto(string const & s, int const base)
{
	T value;
	auto ec = from_chars(s.data(), s.data() + s.size(), value, base).ec;
	if(ec == errc::result_out_of_range)
	{
		stringstream ss;
		ss << "Input value \"" << s << "\" too large for specified numeric type";
		throw out_of_range(ss.str());
	}
	else if(ec == errc::invalid_argument)
	{
		stringstream ss;
		ss << "Input value \"" << s << "\" could not be parsed to a numeric value";
		throw invalid_argument(ss.str());
	}
}

template <typename T> T sto_pos(string const & s, int const base)
{
	T out { sto<T>(s) };
	if(out <= 0)
	{
		throw std::out_of_range("Value must be greater than zero");
	}
	return out;
}

template <typename T> vector<T> stovec(string const & s, char const delim)
{
	vector<T> out;
	istringstream ss { s };
	string line;

	while(getline(ss, line, delim))
		out.push_back(sto<T>(line));

	return out;
}

bool stob(string const & s)
{
	string s_work = s;
	lower(s_work);
	if(s_work == "t" || s_work == "true" || s_work == "1")
		return true;
	if(s_work == "f" || s_work == "false" || s_work == "0")
		return false;
	stringstream ss;
	ss << "Value \"" << s << "\" could not be interpreted as a boolean";
	throw invalid_argument(ss.str());
}

pair<string, string> kvsplit(string const & line, char const delim)
{
	size_t delim_pos;
	delim_pos = line.find(delim);
	if(delim_pos == string::npos)
	{
		stringstream ss;
		ss << "Delimiter '" << delim << "' not found in string \"" << line << "\"";
		throw std::invalid_argument(ss.str());
	}
	return pair<string, string> { line.substr(0, delim_pos),
																line.substr(delim_pos + 1, string::npos) };
}
