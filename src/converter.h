// Copyright (c) 2018, University of New Hampshire, Center for Coastal and Ocean Mapping
#ifndef _CONVERTER_H_
#define _CONVERTER_H_

#include <string>
#include <array>
#include <vector>

typedef struct _t_bagHandle *bagHandle;
typedef int bagError;
typedef struct t_bagReferenceSystem bagReferenceSystem;
typedef struct _t_bag_definition bagDef;
typedef struct _t_bag_data bagData;

namespace bag
{
	class Converter
	{
	public:
		Converter() = default;
		~Converter() = default;

		Converter(const Converter& other) = default;
		Converter(Converter&& other) = default;
		Converter& operator=(const Converter& other) = default;
		Converter& operator=(Converter&& other) = default;

		bool convert(std::string elev_input, std::string bag_output, std::string configdata_path, 
			std::string metadata_path);

		std::string str(size_t spaces = 0) const;

	private:
		bool _read_from_gdal();
		bool _write_to_bag();

		std::string _elev_input;
		std::string _bag_output;
		std::string _configdata_folder;
		std::string _metadata_folder;
		std::string _metadata_template_file;
		std::string _metadata_temp_file;

		std::string _metadata;
		std::vector<float> _elevation;

		std::array<double, 6> _transform{};
		int64_t _rows{};
		int64_t _cols{};
		double _min_x{};
		double _max_x{};
		double _res_x{};
		double _min_y{};
		double _max_y{};
		double _res_y{};
		std::string _hrs;
		std::string _vrs;
		double _geo_s{};
		double _geo_n{};
		double _geo_w{};
		double _geo_e{};
		float _min_depth{};
		float _max_depth{};

		bagHandle _hnd = nullptr;
		bagError _error = 0;
		bagData* _data = nullptr;
	};
}

#endif