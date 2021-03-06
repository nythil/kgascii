/*
    Copyright 2007-2008 Christian Henning, Andreas Pokorny, Lubomir Bourdev
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_EXTENSION_IO_TIFF_TAGS_HPP
#define BOOST_GIL_EXTENSION_IO_TIFF_TAGS_HPP

////////////////////////////////////////////////////////////////////////////////////////
/// \file               
/// \brief All supported tiff tags by the gil io extension.
/// \author Christian Henning, Andreas Pokorny, Lubomir Bourdev \n
///
/// \date   2007-2008 \n
///
////////////////////////////////////////////////////////////////////////////////////////

// taken from jpegxx - https://bitbucket.org/edd/jpegxx/src/ea2492a1a4a6/src/ijg_headers.hpp
#ifndef BOOST_GIL_EXTENSION_IO_TIFF_C_LIB_COMPILED_AS_CPLUSPLUS
    extern "C" {
#else
    // DONT_USE_EXTERN_C introduced in v7 of the IJG library.
    // By default the v7 IJG headers check for __cplusplus being defined and
    // wrap the content in an 'extern "C"' block if it's present.
    // When DONT_USE_EXTERN_C is defined, this wrapping is not performed.
    #ifndef DONT_USE_EXTERN_C
        #define DONT_USE_EXTERN_C 1
    #endif
#endif

#include <tiff.h>

#ifndef BOOST_GIL_EXTENSION_IO_JPEG_C_LIB_COMPILED_AS_CPLUSPLUS
    }
#endif

#include "detail/base.hpp"
#include "formats/tiff/log.hpp"

namespace boost { namespace gil {

/// Defines tiff tag.
struct tiff_tag : format_tag {};

/// http://www.awaresystems.be/imaging/tiff/tifftags/baseline.html
/// http://www.remotesensing.org/libtiff/


/// TIFF property base class
template< typename T, int Value >
struct tiff_property_base : property_base< T >
{
    /// Tag, needed when reading or writing image properties.
    static const unsigned int tag = Value;
};

/// baseline tags

/// Defines type for new subfile property.
struct tiff_new_subfile_type : tiff_property_base< uint32_t, TIFFTAG_SUBFILETYPE > {};

/// Defines type for subfile property.
struct tiff_subfile_type : tiff_property_base< uint16_t, TIFFTAG_OSUBFILETYPE > {};

/// Defines type for image width property.
struct tiff_image_width : tiff_property_base< uint32_t, TIFFTAG_IMAGEWIDTH > {};

/// Defines type for image height property.
struct tiff_image_height : tiff_property_base< uint32_t, TIFFTAG_IMAGELENGTH > {};

/// Defines type for bits per sample property.
struct tiff_bits_per_sample : tiff_property_base< uint16_t, TIFFTAG_BITSPERSAMPLE > {};

/// Defines type for compression property.
struct tiff_compression : tiff_property_base< uint16_t, TIFFTAG_COMPRESSION > {};

/// Defines type for photometric interpretation property.
struct tiff_photometric_interpretation : tiff_property_base< uint16_t, TIFFTAG_PHOTOMETRIC > {};

/// Defines type for threshold property.
struct tiff_thresholding : tiff_property_base< uint16_t, TIFFTAG_THRESHHOLDING > {};

/// Defines type for cell width property.
struct tiff_cell_width : tiff_property_base< uint16_t, TIFFTAG_CELLWIDTH > {};

/// Defines type for cell length property.
struct tiff_cell_length : tiff_property_base< uint16_t, TIFFTAG_CELLLENGTH > {};

/// Defines type for fill order property.
struct tiff_fill_order : tiff_property_base< std::string, TIFFTAG_FILLORDER > {};

/// Defines type for image description.
struct tiff_image_description : tiff_property_base< std::string, TIFFTAG_IMAGEDESCRIPTION > {};

/// Defines type for make property.
struct tiff_make : tiff_property_base< std::string, TIFFTAG_MAKE > {};

/// Defines type for model property.
struct tiff_model : tiff_property_base< std::string, TIFFTAG_MODEL > {};

/// Defines type for image orientation.
struct tiff_orientation : tiff_property_base< uint16_t, TIFFTAG_ORIENTATION > {};

/// Defines type for samples per pixel property.
struct tiff_samples_per_pixel : tiff_property_base< uint16_t, TIFFTAG_SAMPLESPERPIXEL > {};

/// Defines type for rows per strip property.
struct tiff_rows_per_strip : tiff_property_base< uint32_t, TIFFTAG_ROWSPERSTRIP > {};

/// Defines type for min sample property.
struct tiff_min_sample_value : tiff_property_base< uint16_t, TIFFTAG_MINSAMPLEVALUE > {};

/// Defines type for max sample property.
struct tiff_max_sample_value : tiff_property_base< uint16_t, TIFFTAG_MAXSAMPLEVALUE > {};

/// Defines type for x resolution property.
struct tiff_x_resolution : tiff_property_base< float, TIFFTAG_XRESOLUTION > {};

/// Defines type for y resolution property.
struct tiff_y_resolution : tiff_property_base< float, TIFFTAG_YRESOLUTION > {};

/// Defines type for resolution unit property.
struct tiff_resolution_unit : tiff_property_base< uint16_t, TIFFTAG_RESOLUTIONUNIT > {};

/// Defines type for planar configuration property.
struct tiff_planar_configuration : tiff_property_base< uint16_t, TIFFTAG_PLANARCONFIG > {};

/// Defines type for gray response unit property.
struct tiff_gray_response_unit : tiff_property_base< uint16_t, TIFFTAG_GRAYRESPONSEUNIT > {};

/// Defines type for gray response curve property.
struct tiff_gray_response_curve : tiff_property_base< uint16_t*, TIFFTAG_GRAYRESPONSECURVE > {};

/// Defines type for software vendor property.
struct tiff_software : tiff_property_base< std::string, TIFFTAG_SOFTWARE > {};

/// Defines type for date time property.
struct tiff_date_time : tiff_property_base< std::string, TIFFTAG_DATETIME > {};

/// Defines type for artist information property.
struct tiff_artist : tiff_property_base< std::string, TIFFTAG_ARTIST > {};

/// Defines type for host computer property.
struct tiff_host_computer : tiff_property_base< std::string, TIFFTAG_HOSTCOMPUTER > {};

/// Helper structure for reading a color mapper.
struct tiff_color_map
{
   typedef uint16_t* red_t;
   typedef uint16_t* green_t;
   typedef uint16_t* blue_t;

   static const unsigned int tag = TIFFTAG_COLORMAP;
};

/// Defines type for extra samples property.
struct tiff_extra_samples : tiff_property_base< uint16_t*, TIFFTAG_EXTRASAMPLES > {};

/// Defines type for copyright property.
struct tiff_copyright : tiff_property_base< std::string, TIFFTAG_COPYRIGHT > {};

/// non-baseline tags

/// Defines type for sample format property.
struct tiff_sample_format : tiff_property_base< uint16_t, TIFFTAG_SAMPLEFORMAT > {};

/// Defines type for indexed property.
/// Not supported yet
//struct tiff_indexed : tiff_property_base< bool, TIFFTAG_INDEXED > {};

/// Tile related tags

/// Defines type for a (not) tiled tiff image
struct tiff_is_tiled : tiff_property_base< bool, false > {};

/// Defines type for tile width
struct tiff_tile_width : tiff_property_base< long, TIFFTAG_TILEWIDTH > {};

/// Defines type for tile length
struct tiff_tile_length : tiff_property_base< long, TIFFTAG_TILELENGTH > {};

/// Defines the page to read in a multipage tiff file.
#include <boost/mpl/integral_c.hpp>
struct tiff_directory : property_base< tdir_t >
{
    typedef boost::mpl::integral_c< type, 0 > default_value;
};

/// Read information for tiff images.
///
/// The structure is returned when using read_image_info.
template<>
struct image_read_info< tiff_tag >
{
    image_read_info()
    : _width( 0 )
    , _height( 0 )

    , _compression( COMPRESSION_NONE )

    , _bits_per_sample( 0 )
    , _samples_per_pixel( 0 )
    , _sample_format( SAMPLEFORMAT_UINT )

    , _planar_configuration( PLANARCONFIG_CONTIG )

    , _photometric_interpretation( PHOTOMETRIC_MINISWHITE )

    , _is_tiled( false )

    , _tile_width ( 0 )
    , _tile_length( 0 )
    {}

    /// The number of rows of pixels in the image.
    tiff_image_width::type  _width;
    /// The number of columns in the image, i.e., the number of pixels per row.
    tiff_image_height::type _height;

    /// Compression scheme used on the image data.
    tiff_compression::type _compression;

    /// Number of bits per component.
    tiff_bits_per_sample::type   _bits_per_sample;
    /// The number of components per pixel.
    tiff_samples_per_pixel::type _samples_per_pixel;
    /// Specifies how to interpret each data sample in a pixel.
    tiff_sample_format::type     _sample_format;

    /// How the components of each pixel are stored.
    tiff_planar_configuration::type _planar_configuration;

    /// The color space of the image data.
    tiff_photometric_interpretation::type _photometric_interpretation;

    /// Is tiled?
    tiff_is_tiled::type _is_tiled;
    /// Tile width
    tiff_tile_width::type _tile_width;
    /// Tile length
    tiff_tile_length::type _tile_length;
};

/// Read settings for tiff images.
///
/// The structure can be used for all read_xxx functions, except read_image_info.
template<>
struct image_read_settings< tiff_tag > : public image_read_settings_base
{
    /// Default constructor
    image_read_settings< tiff_tag >()
    : image_read_settings_base()
    , _directory( tiff_directory::default_value::value )
    {}

    /// Constructor
    /// \param top_left  Top left coordinate for reading partial image.
    /// \param dim       Dimensions for reading partial image.
    /// \param directory Defines the page to read in a multipage tiff file.
    image_read_settings( const point_t&              top_left
                       , const point_t&              dim
                       , const tiff_directory::type& directory = tiff_directory::default_value::value
                       )
    : image_read_settings_base( top_left
                              , dim
                              )
    , _directory( directory )
    {}

    /// Defines the page to read in a multipage tiff file.
    tiff_directory::type _directory;
};

/// Read settings for tiff images.
///
/// The structure can be used for all read_xxx functions, except read_image_info.
template< typename Log >
struct image_write_info< tiff_tag, Log >
{
    /// Default constructor
    image_write_info()
    : _photometric_interpretation( PHOTOMETRIC_MINISBLACK )
    , _compression               ( COMPRESSION_NONE       )
    , _orientation               ( ORIENTATION_TOPLEFT    )
    , _planar_configuration      ( PLANARCONFIG_CONTIG    )
    , _is_tiled                  ( false )
    , _tile_width                ( 0 )
    , _tile_length               ( 0 )
    , _x_resolution              ( 0 )
    , _y_resolution              ( 0 )
    {}

    /// The color space of the image data.
    tiff_photometric_interpretation::type _photometric_interpretation;
    /// Compression scheme used on the image data.
    tiff_compression::type                _compression;
    /// The orientation of the image with respect to the rows and columns.
    tiff_orientation::type                _orientation;
    /// How the components of each pixel are stored.
    tiff_planar_configuration::type       _planar_configuration;

    /// Is the image tiled?
    tiff_is_tiled::type                   _is_tiled;
    /// Tiles width
    tiff_tile_width::type                 _tile_width;
    /// Tiles length
    tiff_tile_length::type                _tile_length;

    /// x, y resolution
    tiff_x_resolution::type               _x_resolution;
    tiff_y_resolution::type               _y_resolution;

    /// A log to transcript error and warning messages issued by libtiff.
    Log                                   _log;
};

} // namespace gil
} // namespace boost

#endif // BOOST_GIL_EXTENSION_IO_TIFF_TAGS_HPP
