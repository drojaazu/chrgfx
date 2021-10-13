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

		string id() const
		{
			return m_id;
		};

	protected:
		gfxdef(string const & id) : m_id(id)
		{
#ifdef DEBUG
			std::cerr << "NEW GFXDEF: " << id << std::endl;
#endif
		};

	private:
		string m_id;
	};

} // namespace chrgfx

#endif
