
#include "defblocks.hpp"

static constexpr char COMMENT_MARKER = '#';
static constexpr char BLOCK_OPENER = '{';
static constexpr char BLOCK_CLOSER = '}';
static constexpr char KV_DELIM = ' ';

string ltrim(string const &s)
{
	size_t start = s.find_first_not_of(' ');
	return (start == string::npos) ? "" : s.substr(start);
}

string rtrim(string const &s)
{
	size_t end = s.find_last_not_of(' ');
	return (end == string::npos) ? "" : s.substr(0, end + 1);
}

string trim(string const &s) { return rtrim(ltrim(s)); }

// assumes the string passed in has already had any whitespace trimmed
pair<string, string> kvsplit(string const &line)
{
	std::size_t spacedelim_pos;
	spacedelim_pos = line.find(KV_DELIM);
	if(spacedelim_pos == string::npos) {
		throw std::invalid_argument(
				"Could not find specified delimiter in given string");
	}
	return pair<string, string>{line.substr(0, spacedelim_pos),
															line.substr(spacedelim_pos + 1, string::npos)};
}

defblock parse_defblock(std::istream &in)
{

	if(!in.good()) {
		throw std::ios_base::failure(std::strerror(errno));
	}

	string opener_check;
	std::getline(in, opener_check);
	std::replace(opener_check.begin(), opener_check.end(), '\t', ' ');
	opener_check = trim(opener_check);
	if(opener_check[0] != BLOCK_OPENER || opener_check.size() != 1) {
		throw "Block opening delimiter not found or junk data found on opener line";
	}

	string this_line;

	defblock out;

	while(std::getline(in, this_line)) {
		std::replace(this_line.begin(), this_line.end(), '\t', ' ');
		this_line = trim(this_line);
		if(this_line == "" || this_line[0] == COMMENT_MARKER) {
			// ignore comment & empty lines
			continue;
		}

		// end of block
		if(this_line[0] == BLOCK_CLOSER) {

			break;
		}

		// didn't find a closer
		if(in.eof()) {
			throw "Hit end of file before end of block";
		}

		// no issues, just a normal line, clean and append to the block
		out.insert(kvsplit(this_line));
	}

	return out;
}

std::multimap<string const, defblock const> load_defblocks(string const &file)
{
	std::ifstream in{file};
	if(!in.good()) {
		throw std::ios_base::failure(std::strerror(errno));
	}

	in.seekg(std::ios::beg);

	string this_line, work_line, cached_line, this_def_name, this_block;

	std::multimap<string const, defblock const> out;

	while(std::getline(in, this_line)) {
		work_line = this_line;
		std::replace(work_line.begin(), work_line.end(), '\t', ' ');
		work_line = ltrim(this_line);

		// blank line or a comment; move on
		if(work_line == "" || work_line[0] == COMMENT_MARKER) {
			continue;
		}

		// block delimiters cannot be used in declaration name
		// (or syntax error in the file)
		if(work_line.find(BLOCK_OPENER) != string::npos ||
			 work_line.find(BLOCK_CLOSER) != string::npos) {
			std::cerr << "Encountered block delimiter before block declaration"
								<< std::endl;
			continue;
		}

		// assume any text we've found is a block declaration
		this_def_name = rtrim(work_line);
		// stream pointer should now be ready at the next line
		try {
			defblock this_defblock = parse_defblock(in);
			out.insert({this_def_name, this_defblock});
		} catch(string const &e) {
			std::cerr << e << std::endl;
			continue;
		}
	}
	return out;
}
