#include "cfgload.hpp"
#include "lineread.hpp"
#include "strutil.hpp"
#include <stdexcept>
#include <string>

namespace motoi
{
using namespace std;

file_parse_error::file_parse_error(char const * message, size_t line_number) :
		std::runtime_error(std::string(message) + " at line " + std::to_string(line_number))
{
}

config_loader::config_loader(string const & filepath) :
		m_file(filepath)
{
	m_file.reset();
	m_parse_state = parse_state::search_header;
	string block_header;
	block_map * block_content {nullptr};
	while (m_file.next())
	{
		if (m_file.line().empty())
			continue;

		auto line {trim_view(m_file.line())};
		if (line.empty() || line[0] == COMMENT_MARKER)
			continue;

		switch (m_parse_state)
		{
			case parse_state::search_header:
			{
				if (line[0] == BLOCK_OPENER || line[0] == BLOCK_CLOSER)
				{
					throw file_parse_error("Found block opener/closer with no header", m_file.line_number());
					break;
				}

				// at this point, we should have found some text which represents a block header
				// (because blank lines and comments are dealt with above)
				// we want to verify there is a block opener before actyually allocating a new map, however
				block_header = line;
				m_parse_state = parse_state::search_opener;
				break;
			}

			case parse_state::search_opener:
			{
				if (line[0] == BLOCK_OPENER)
				{
					block_content = &this->emplace(block_header, block_map())->second;
					m_parse_state = parse_state::in_block;
					break;
				}

				if (line[0] == BLOCK_CLOSER)
				{
					throw file_parse_error("Block closer found without matching opener", m_file.line_number());
					break;
				}

				throw file_parse_error("Found text outside of config block", m_file.line_number());
				break;
			}

			case parse_state::search_closer:
			{
				if (line[0] == BLOCK_CLOSER)
				{
					m_parse_state = parse_state::search_header;
					block_content = nullptr;
					block_header.clear();
					break;
				}

				if (line[0] == BLOCK_OPENER)
				{
					throw file_parse_error("Block opener found without matching closer", m_file.line_number());
					break;
				}
				break;
			}

			case parse_state::in_block:
			{
				if (line[0] == BLOCK_CLOSER)
				{
					m_parse_state = parse_state::search_header;
					block_content = nullptr;
					block_header.clear();
					break;
				}

				// kvsplit does check for presence of delimiter, throws if not found
				try
				{
					block_content->emplace(kvsplit(basic_string<char>(line), "\t "));
				}
				catch (invalid_argument const & e)
				{
					throw file_parse_error("Could not find entry delimiter", m_file.line_number());
				}
				break;
			}
		}
	}
}
} // namespace motoi