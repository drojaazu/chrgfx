#include "chrbank.hpp"

namespace chrgfx
{
chrbank::chrbank(chrdef def) : def(std::move(def)){};

chrdef chrbank::get_chrdef() const { return this->def; }

} // namespace chrgfx
