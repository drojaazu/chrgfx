#include "xdgdirs.hpp"

namespace motoi
{

std::string xdg_data_dirs()
{
	auto xdg_data_dirs = getenv("XDG_DATA_DIRS");
	if (xdg_data_dirs == nullptr || strlen(xdg_data_dirs) == 0)
		return "/usr/local/share:/usr/share";
	return xdg_data_dirs;
}

std::string xdg_data_home()
{
	auto xdg_data_home {getenv("XDG_DATA_HOME")};
	if (xdg_data_home == nullptr || strlen(xdg_data_home) == 0)
	{
		auto const home_path = getenv("HOME");
		if (home_path == nullptr || strlen(home_path) == 0)
			throw std::runtime_error("could not get value of $HOME environment variable");
		return concat_paths(std::string(home_path), std::string("/.local/share"));
	}
	return xdg_data_home;
}

std::string xdg_config_dirs()
{
	auto xdg_config_dirs {getenv("XDG_CONFIG_DIRS")};
	if (xdg_config_dirs == nullptr || strlen(xdg_config_dirs) == 0)
		return "/etc/xdg";
	return xdg_config_dirs;
}

std::string xdg_config_home()
{
	auto xdg_config_home {getenv("XDG_CONFIG_HOME")};
	if (xdg_config_home == nullptr || strlen(xdg_config_home) == 0)
	{
		std::string home_path = getenv("HOME");
		if (home_path.empty())
			throw std::runtime_error("could not get value of $HOME environment variable");
		return concat_paths(home_path, std::string("/.config"));
	}
	return xdg_config_home;
}

std::vector<std::string> data_filepaths(char const * filename)
{
	return data_filepaths(std::string(filename));
}

std::vector<std::string> config_filepaths(char const * filename)
{
	return config_filepaths(std::string(filename));
}

} // namespace motoi
