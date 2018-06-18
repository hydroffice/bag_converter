#include "converter.h"
#include "helper.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <iomanip>

#ifdef _MSC_VER
#pragma warning(disable : 4251)  // to avoid "needs to have dll-interface to be used by clients of class"
#endif
#include "gdal_priv.h"
#include "ogr_spatialref.h"
#include "cpl_conv.h"
#include "bag.h"
#include <memory>
#include <utility>


std::string bag_error_str( const bagError& err, size_t spaces = 0 )
{
	uint8_t* u8_str = nullptr;
	bagGetErrorString( err, &u8_str );
	return std::string( reinterpret_cast<char*>( u8_str ) );
}

bool bag::Converter::convert(std::string elev_input, std::string bag_output, std::string configdata_path, std::string metadata_path )
{
	_elev_input = std::move(elev_input);
	_bag_output = std::move(bag_output);
	_configdata_folder = std::move(configdata_path);
	_metadata_folder = std::move(metadata_path);

	bool success = _read_from_gdal();
	if(!success)
	{
		return false;
	}

	return _write_to_bag();
}

std::string bag::Converter::str(size_t spaces) const
{
	std::stringstream ss;
	std::string spc(spaces, ' ');
	std::string spc2( spaces + 2, ' ' );

	ss << spc << "Converter: " << std::endl;

	ss << spc2 << "input elevation: " << _elev_input << std::endl;
	ss << spc2 << "output bag: " << _bag_output << std::endl;
	
	return ss.str();
}

bool bag::Converter::_read_from_gdal()
{
	std::cout << "Reading with GDAL library: " << GDALVersionInfo(nullptr) << std::endl;

	// Open the elevation input

	GDALAllRegister();
	auto poDataset = static_cast<GDALDataset *>(GDALOpen(_elev_input.c_str(), GA_ReadOnly));
	if(poDataset == nullptr)
	{
		std::cerr << "unable to open input file: " << _elev_input;
		return false;
	}
	std::cout << " * Driver: " << poDataset->GetDriver()->GetDescription() << std::endl;


	// Retrieve info and metadata

	_rows = poDataset->GetRasterYSize();
	_cols = poDataset->GetRasterXSize();
	std::cout << " * Size: " << _rows << "x" << _cols << std::endl;

	poDataset->GetGeoTransform( _transform.data() );
	_res_x = abs( _transform[1] );
	_min_x = _transform[0] + _res_x / 2.0;
	_max_x = _min_x + ( _cols - 1.0 ) * _res_x;
	_res_y = abs( _transform[5] );
	_max_y = _transform[3] - _res_y / 2.0;
	_min_y = _max_y - ( _rows - 1.0 ) * _res_y;

	_hrs = poDataset->GetProjectionRef();
	// std::cout << "hrs: " << _hrs << std::endl;
	_vrs = R"(VERT_CS["//", VERT_DATUM["//", 2000]])";


	// calculate the geographic bounding box

	OGRSpatialReference oSourceSRS, oTargetSRS;
	auto pszWkt = const_cast<char*>(poDataset->GetProjectionRef());
	oSourceSRS.importFromWkt( &pszWkt );
	oTargetSRS.importFromEPSG( 4326 ); // WGS84 / Geographic
	OGRCoordinateTransformation *poCT = OGRCreateCoordinateTransformation( &oSourceSRS, &oTargetSRS );
	double x;
	double y;
	x = _min_x;
	y = _min_y;
	poCT->Transform( 1, &x, &y );
	_geo_w = x;
	_geo_s = y;

	x = _max_x;
	y = _max_y;
	poCT->Transform( 1, &x, &y );
	_geo_e = x;
	_geo_n = y;


	// Read all the data at once and replace 'nodata' values

	float out_nodata = 1000000.0f;
	GDALRasterBand *poBand = poDataset->GetRasterBand( 1 );
	double in_nodata = poBand->GetNoDataValue( nullptr );
	std::cout << " * No-data value: " << std::fixed << in_nodata << std::endl;
	_elevation.resize( _rows*_cols );
	CPLErr ret = poBand->RasterIO( GF_Read, 0, 0, int( _cols ), int( _rows ), static_cast<void*>(_elevation.data()),
		int( _cols ), int( _rows ), GDT_Float32, 0, 0 );
	if(ret == CE_Failure)
	{
		std::cerr << "output raster IO issue" << std::endl;
		exit( -1 );
	}
	ret = poBand->FlushCache();
	if(ret == CE_Failure)
	{
		std::cerr << "output flush cache issue" << std::endl;
		exit( -1 );
	}
	std::replace( _elevation.begin(), _elevation.end(), float( in_nodata ), out_nodata );


	// Retrieve min/max elevation

	double adfMinMax[2];
	GDALComputeRasterMinMax( GDALRasterBandH(poBand), TRUE, adfMinMax );
	_min_depth = float( adfMinMax[0] );
	_max_depth = float( adfMinMax[1] );

	GDALClose( poDataset );

	return true;
}

bool bag::Converter::_write_to_bag()
{
	// Check if the output already exist

	if(path_exists( _bag_output ))
	{
		remove_path( _bag_output );
	}

	std::cout << "Writing with BAG library: " << BAG_VERSION << std::endl;


	// Manage the 'configdata' folder

	if(_configdata_folder.empty())
	{
		// C-string since a null pointer is underfined behavior for string
		const char* buffer = getenv( "BAG_HOME" );
		if(buffer)
		{
			_configdata_folder = buffer;

			if(!file_exists( _configdata_folder, "ellips.dat" ))
			{
				std::cerr << "BAG_HOME is set, but invalid: " << _configdata_folder << std::endl;
				return false;
			}
		}

		std::cerr << "BAG_HOME is not set." << std::endl;
		return false;
	}
	int ret_value = setenv( "BAG_HOME", _configdata_folder.c_str(), 1 );
	if(ret_value != 0)
	{
		std::cerr << "issue with setenv: " << ret_value << " using " << _configdata_folder << std::endl;
		return false;
	}
	std::cout << " * BAG_HOME: " << getenv( "BAG_HOME" ) << std::endl;


	// Read the metadata template

	_metadata_template_file = join_path( _metadata_folder, "metadata_template.xml" );
	if(!path_exists( _metadata_template_file ))
	{
		std::cerr << "issue in locating the template path: " << _metadata_template_file << std::endl;
		return false;
	}


	// Populate the metadata

	_metadata_temp_file = replace_str( _bag_output, ".bag", ".xml" );
	_metadata = ascii_file_content( _metadata_template_file );
	_metadata = replace_str( _metadata, "BAGCNV_UNIQUE_ID", unique_id() );
	_metadata = replace_str( _metadata, "BAGCNV_RESPONSIBLE_PARTY", "BAG Converter" );
	_metadata = replace_str( _metadata, "BAGCNV_CURRENT_DATE", cur_iso_date() );
	_metadata = replace_str( _metadata, "BAGCNV_ROW_COUNT", std::to_string( _rows ) );
	_metadata = replace_str( _metadata, "BAGCNV_ROW_SPACING", std::to_string( _res_y ) );
	_metadata = replace_str( _metadata, "BAGCNV_COL_COUNT", std::to_string( _cols ) );
	_metadata = replace_str( _metadata, "BAGCNV_COL_SPACING", std::to_string( _res_x ) );
	std::stringstream ss_corned_coords;
	ss_corned_coords << std::fixed << std::setprecision( 8 ) << _min_x << "," << _min_y << " " << _max_x << "," << _max_y;
	_metadata = replace_str( _metadata, "BAGCNV_CORNER_COORDS", ss_corned_coords.str() );
	_metadata = replace_str( _metadata, "BAGCNV_HRS", _hrs );
	_metadata = replace_str( _metadata, "BAGCNV_VRS", _vrs );
	_metadata = replace_str( _metadata, "BAGCNV_DATASET_TITLE", "Grid output" );
	_metadata = replace_str( _metadata, "BAGCNV_DATASET_DATE", cur_iso_date() );
	_metadata = replace_str( _metadata, "BAGCNV_INPUT_RESPONSIBLE_PARTY", "BAG Converter" );
	_metadata = replace_str( _metadata, "BAGCNV_ABSTRACT", "Created by BAG Converter" );
	std::stringstream ss_geo_w;
	ss_geo_w << std::fixed << std::setprecision( 8 ) << _geo_w;
	_metadata = replace_str( _metadata, "BAGCNV_WEST_LONGITUDE", ss_geo_w.str() );
	std::stringstream ss_geo_e;
	ss_geo_e << std::fixed << std::setprecision( 8 ) << _geo_e;
	_metadata = replace_str( _metadata, "BAGCNV_EAST_LONGITUDE", ss_geo_e.str() );
	std::stringstream ss_geo_s;
	ss_geo_s << std::fixed << std::setprecision( 8 ) << _geo_s;
	_metadata = replace_str( _metadata, "BAGCNV_SOUTH_LATITUDE", ss_geo_s.str() );
	std::stringstream ss_geo_n;
	ss_geo_n << std::fixed << std::setprecision( 8 ) << _geo_n;
	_metadata = replace_str( _metadata, "BAGCNV_NORTH_LATITUDE", ss_geo_n.str() );
	_metadata = replace_str( _metadata, "BAGCNV_LINEAGE_DESC", "Created by BAG Converter" );
	_metadata = replace_str( _metadata, "BAGCNV_LINEAGE_DATETIME", cur_iso_timestamp() );
	_metadata = replace_str( _metadata, "BAGCNV_LINEAGE_RESPONSIBLE_PARTY", "BAG Converter" );


	// Write the temporary metadata file

	std::ofstream ofs( _metadata_temp_file );
	if(!ofs)
	{
		std::cerr << "Unable to open output file: " << _metadata_temp_file << std::endl;
		return false;
	}
	ofs << _metadata;
	ofs.close();


	// Create the BAG file

	_data = new bagData;
	std::unique_ptr<bagData> p_data( _data );
	memset( _data, 0, sizeof( bagData ) );

	_error = bagInitDefinitionFromFile( _data, const_cast<char*>( _metadata_temp_file.c_str() ) );
	if(_error != BAG_SUCCESS)
	{
		std::cerr << "While init definition from " << _metadata_temp_file << ": " << bag_error_str( _error ) << std::endl;
		return false;
	}
	_data->min_elevation = _min_depth;
	_data->max_elevation = _max_depth;
	std::cout << std::fixed << " * Elevation range: " << _data->min_elevation << ", " << _data->max_elevation << std::endl;

	_data->min_uncertainty = BAG_NULL_UNCERTAINTY;
	_data->max_uncertainty = BAG_NULL_UNCERTAINTY;

	_data->compressionLevel = 1;

	_error = bagFileCreate( reinterpret_cast<const uint8_t *>( _bag_output.c_str() ), _data, &_hnd );
	if(_error != BAG_SUCCESS)
	{
		std::cerr << "While opening " << _bag_output << ": " << bag_error_str( _error ) << std::endl;
		return false;
	}

	// Populate layers row-by-row

	std::vector<float> uncertainty( _cols, BAG_NULL_UNCERTAINTY );

	for(int32_t r = 0; r < int32_t( _rows ); r++)
	{
		_error = bagWriteRow( _hnd, int32_t( _rows ) - r - 1, 0, int32_t( _cols ) - 1, Elevation,
			static_cast<void*>( _elevation.data() + r*_cols ) );
		if(_error != BAG_SUCCESS)
		{
			std::cerr << "While writing elevation at row " << r << ": " << bag_error_str( _error ) << std::endl;
			return false;
		}
		_error = bagWriteRow( _hnd, r, 0, int32_t( _cols ) - 1, Uncertainty,
			static_cast<void*>( uncertainty.data() ) );
		if(_error != BAG_SUCCESS)
		{
			std::cerr << "While writing uncertainty at row " << r << ": " << bag_error_str( _error ) << std::endl;
			return false;
		}
	}

	// Cleaning stuff

	remove_path( _metadata_temp_file );
	_elevation.clear();

	return true;
}
