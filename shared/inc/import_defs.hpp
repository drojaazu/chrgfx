#ifndef CHRGFX_EXEC_IMPORT_DEFS_H
#define CHRGFX_EXEC_IMPORT_DEFS_H

#include "chrgfx.hpp"

std::map<std::string, chrgfx::chr_def> load_chrdefs(const std::string def_file);

std::map<std::string, chrgfx::col_def> load_coldefs(const std::string def_file);

std::map<std::string, chrgfx::pal_def> load_paldefs(const std::string def_file);

#endif