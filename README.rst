BAG Converter
=============

.. image:: https://github.com/hydroffice/bag_converter/raw/master/favicon.png
    :alt: logo

	
General Info
------------

.. image:: https://api.codacy.com/project/badge/Grade/d1e89bbb0211432bb7bf12ccefa62b04
    :target: https://www.codacy.com/app/hydroffice/bag_converter
    :alt: Codacy

A simple converter from GDAL raster formats to BAG format.


Dependencies
------------

For the library, you will need:

* `GDAL <http://www.gdal.org/>`_
* `BAG <http://www.opennavsurf.org/index.html>`_


How to build
------------

The build is based on `CMake <https://cmake.org/>`_.

The Cmake's option 'BAGCNV_USE_INTERNAL_BINS' (only available on Windows) links against internally-shipped binary libraries for GDAL and BAG.


How to use
----------

Run *bag_converter [GDAL raster input] [BAG output]*.

The *[GDAL raster input]* is a path to a raster file (e.g., GeoTiff) that will be used to populate the elevation layer in the BAG file.

The *[BAG output]* provides name and path for the generated BAG.


Other info
----------

* GitHub: `https://github.com/hydroffice/bag_converter <https://github.com/hydroffice/bag_converter>`_
* Project page: `http://www.hydroffice.org/bag/ <http://www.hydroffice.org/bag/>`_
* License: LGPLv3 license (See `LICENSE <https://github.com/hydroffice/bag_converter/raw/master/LICENSE>`_)
