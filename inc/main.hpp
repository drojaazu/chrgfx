#include <getopt.h>
#include <stdio.h>
#include <iostream>
#include <map>

#include "1bpp_gfx.hpp"
#include "capcom_cps_gfx.hpp"
#include "nintendo_fc_gfx.hpp"
#include "nintendo_gb_gfx.hpp"
#include "nintendo_sfc_gfx.hpp"
#include "nintendo_vb.hpp"
#include "render.hpp"
#include "sega_8bit_gfx.hpp"
#include "sega_md_gfx.hpp"
#include "snk_neogeo.hpp"
#include "snk_ngp_gfx.hpp"
#include "tlp_palette.hpp"

void process_args(int argc, char** argv);
void print_help();
void free_vectors();