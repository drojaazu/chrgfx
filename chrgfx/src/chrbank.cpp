#include "chrbank.hpp"

namespace chrgfx
{

chrbank::chrbank(chrdef const &def) : def(std::move(def)){};

chrbank::~chrbank()
{
	// pretty sure we don't need to worry about the rule of three/five here
	// since the underlying vector is managing most everything
	// we just went to make sure all the pointers get deleted
	for(auto it = this->begin(); it != this->end(); it++)
		delete[] * it;
};

chrdef chrbank::get_chrdef() { return this->def; }

} // namespace chrgfx
