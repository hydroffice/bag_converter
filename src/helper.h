#ifndef _BAG_CONVERTER_HELPER_H_
#define _BAG_CONVERTER_HELPER_H_
#include <string>


#if __STDC_LIB_EXT1__ || _MSC_VER
int setenv( const char* name, const char* value, int overwrite );
#endif

namespace bag
{
	void msvc_pause(bool skip = false);

	std::string cur_iso_timestamp();
	std::string cur_iso_date();
	std::string unique_id();

	std::string replace_str( std::string str, const std::string& search = "\\", const std::string& replace = "/" );

	std::string ascii_file_content( const std::string &path );

	std::string join_path( const std::string& first_part, const std::string& second_part );
	std::string folder_path( const std::string& path );
	bool file_exists( const std::string& folder_path, const std::string& filename );
	bool path_exists( const std::string& full_path );
	bool remove_path( const std::string& path );

	std::string cwd();
}

#endif