#include "helper.h"

#include <cstdlib>
#include <string>
#include <experimental/filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <random>

namespace fs = std::experimental::filesystem;

#if __STDC_LIB_EXT1__ || _MSC_VER
int setenv( const char* name, const char* value, int overwrite )
{
	if(!overwrite)
	{
		size_t envsize = 0;
		int errcode = getenv_s( &envsize, nullptr, 0, name );
		if(errcode || envsize)
		{
			return errcode;
		}
	}
	return _putenv_s( name, value );
}
#endif

void bag::msvc_pause( bool skip )
{
	if(skip)
	{
		return;
	}
#ifdef _MSC_VER
	system( "pause" );
#endif
}

std::string bag::cur_iso_timestamp()
{
	time_t t = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
	std::stringstream ss;
	ss << std::put_time( std::gmtime( &t ), "%Y-%m-%dT%TZ" );
	return ss.str();
}

std::string bag::cur_iso_date()
{
	time_t t = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
	std::stringstream ss;
	ss << std::put_time( std::gmtime( &t ), "%Y-%m-%d" );
	return ss.str();
}

std::string bag::unique_id()
{
	time_t t = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );

	std::mt19937 rng;
	rng.seed( std::random_device()( ) );
	std::uniform_int_distribution<std::mt19937::result_type> dist( 0, 999999 );

	std::stringstream ss;
	ss << t << "-" << std::setfill( '0' ) << std::setw( 4 ) << dist( rng );
	return ss.str();
}

std::string bag::replace_str( std::string str, const std::string& search, const std::string& replace )
{
	// prevents bad alloc issues
	if(search.empty())
	{
		return str;
	}

	size_t pos = 0;
	while(( pos = str.find( search, pos ) ) != std::string::npos)
	{
		str.replace( pos, search.length(), replace );
		pos += replace.length();
	}
	return str;
}

std::string bag::ascii_file_content(const std::string& path)
{
	std::ifstream in( path.c_str(), std::ios::in | std::ios::binary );
	if(!in)
	{
		throw std::runtime_error( "unable to open: " + path );
	}

	std::string content;
	in.seekg( 0, std::ios::end );
	content.resize( static_cast<size_t>( in.tellg() ) );
	in.seekg( 0, std::ios::beg );
	in.read( &content[0], content.size() );
	in.clear();

	return content;
}

bool bag::path_exists( const std::string& full_path )
{
	return fs::exists( full_path );
}

bool bag::remove_path( const std::string& path )
{
	return fs::remove( path );
}

std::string bag::cwd()
{
	return fs::current_path().string();
}

std::string bag::join_path( const std::string& first_part, const std::string& second_part )
{
	fs::path first( first_part );
	fs::path second( second_part );
	fs::path full_path = first / second;
	return replace_str( full_path.string() );
}

std::string bag::folder_path( const std::string& path )
{
	if(!path_exists( path ))
	{
		throw std::runtime_error( "Path does not exist or not accessible: " + path );
	}
	fs::path _path( path );
	return replace_str( _path.parent_path().string() );
}

bool bag::file_exists(const std::string& folder_path, const std::string& filename)
{
	return path_exists( join_path( folder_path, filename ) );
}
