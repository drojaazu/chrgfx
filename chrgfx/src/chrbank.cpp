#include "chrbank.hpp"

namespace chrgfx
{
chrbank::chrbank(chrdef const &def) : def(std::move(def)){};

chrdef chrbank::get_chrdef() { return this->def; }

} // namespace chrgfx
