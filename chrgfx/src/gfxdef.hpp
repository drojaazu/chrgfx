#ifndef CHRGFX__GFXDEF_H
#define CHRGFX__GFXDEF_H

#include <iostream>
#include <string>

using std::string;

namespace chrgfx
{

	/**
	 * \class gfxdef
	 * \brief Abstract graphics definition class
	 */
	class gfxdef
	{
	public:
		gfxdef() = delete;
		gfxdef(gfxdef &&) = default;
		gfxdef(gfxdef const &) = default;
		gfxdef & operator=(gfxdef const &) = delete;
		gfxdef & operator=(gfxdef &&) = delete;

		string get_id() const
		{
			return id;
		};

	protected:
		gfxdef(string const & id) : id(id)
		{
#ifdef DEBUG
			std::cerr << "NEW GFXDEF: " << id << std::endl;
#endif
		};

	private:
		string id;
	};

} // namespace chrgfx

#endif
