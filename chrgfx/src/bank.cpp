#include "bank.hpp"


namespace chrgfx{

bank::bank(chr_def const &chrdef) : chrdef(std::move(chrdef)){};

bank::~bank() 
{
		for(auto it = this->begin(); it != this->end(); ++it)
			delete[] & it;
};

chr_def bank::get_chrdef() { return this->chrdef; }


} // namespace chrgfx
