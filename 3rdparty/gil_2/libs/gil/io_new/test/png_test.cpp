//#define BOOST_TEST_MODULE png_test_module
#include <boost/test/unit_test.hpp>

#include <fstream>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io_new/png_all.hpp>

#include <boost/type_traits/is_same.hpp>

#include "mandel_view.hpp"
#include "paths.hpp"
#include "subimage_test.hpp"

using namespace std;
using namespace boost;
using namespace gil;

typedef png_tag tag_t;

BOOST_AUTO_TEST_SUITE( png_test )

BOOST_AUTO_TEST_CASE( read_image_info_using_string )
{
    {
        image_read_info< png_tag > info = read_image_info( png_filename
                                                         , tag_t() );
        BOOST_CHECK_EQUAL( info._width , 320u );
        BOOST_CHECK_EQUAL( info._height, 240u );
    }

    {
        ifstream in( png_filename.c_str(), ios::binary );

        image_read_info< tag_t > info = read_image_info( in
                                                       , tag_t() );

        BOOST_CHECK_EQUAL( info._width , 320u );
        BOOST_CHECK_EQUAL( info._height, 240u );
    }

    {
        FILE* file = fopen( png_filename.c_str(), "rb" );
        
        image_read_info< tag_t > info = read_image_info( file
                                                       , tag_t() );

        BOOST_CHECK_EQUAL( info._width , 320u );
        BOOST_CHECK_EQUAL( info._height, 240u );
    }
}

BOOST_AUTO_TEST_CASE( read_image_test )
{
    {
        rgba8_image_t img;
        read_image( png_filename, img, tag_t() );

        BOOST_CHECK_EQUAL( img.width() , 320u );
        BOOST_CHECK_EQUAL( img.height(), 240u );
    }

    {
        ifstream in( png_filename.c_str(), ios::binary );

        rgba8_image_t img;
        read_image( in, img, tag_t() );

        BOOST_CHECK_EQUAL( img.width() , 320u );
        BOOST_CHECK_EQUAL( img.height(), 240u );
    }

    {
        FILE* file = fopen( png_filename.c_str(), "rb" );
        
        rgba8_image_t img;
        read_image( file, img, tag_t() );

        BOOST_CHECK_EQUAL( img.width() , 320u );
        BOOST_CHECK_EQUAL( img.height(), 240u );
    }
}

BOOST_AUTO_TEST_CASE( read_and_convert_image_test )
{
    {
        rgb8_image_t img;
        read_and_convert_image( png_filename, img, tag_t() );

        BOOST_CHECK_EQUAL( img.width() , 320u );
        BOOST_CHECK_EQUAL( img.height(), 240u );
    }

    {
        rgba8_image_t img;
        read_and_convert_image( png_filename, img, tag_t() );

        BOOST_CHECK_EQUAL( img.width() , 320u );
        BOOST_CHECK_EQUAL( img.height(), 240u );
    }

    {
        ifstream in( png_filename.c_str(), ios::binary );

        rgb8_image_t img;
        read_and_convert_image( in, img, tag_t() );

        BOOST_CHECK_EQUAL( img.width() , 320u );
        BOOST_CHECK_EQUAL( img.height(), 240u );
    }

    {
        FILE* file = fopen( png_filename.c_str(), "rb" );
        
        rgb8_image_t img;
        read_and_convert_image( file, img, tag_t() );

        BOOST_CHECK_EQUAL( img.width() , 320u );
        BOOST_CHECK_EQUAL( img.height(), 240u );
    }
}

BOOST_AUTO_TEST_CASE( read_view_test )
{
    {
        rgba8_image_t img( 320, 240 );
        read_view( png_filename, view( img ), tag_t() );
    }

    {
        ifstream in( png_filename.c_str(), ios::binary );

        rgba8_image_t img( 320, 240 );
        read_view( in, view( img ), tag_t() );
    }

    {
        FILE* file = fopen( png_filename.c_str(), "rb" );
        
        rgba8_image_t img( 320, 240 );
        read_view( file, view( img ), tag_t() );
    }
}

BOOST_AUTO_TEST_CASE( read_and_convert_view_test )
{
    {
        rgb8_image_t img( 320, 240 );
        read_and_convert_view( png_filename, view( img ), tag_t() );
    }

    {
        ifstream in( png_filename.c_str(), ios::binary );

        rgb8_image_t img( 320, 240 );
        read_and_convert_view( in, view( img ), tag_t() );
    }

    {
        FILE* file = fopen( png_filename.c_str(), "rb" );
        
        rgb8_image_t img( 320, 240 );

        read_and_convert_view( file
                             , view( img )
                             , tag_t()
                             );
    }
}

BOOST_AUTO_TEST_CASE( write_view_test )
{
    {
        string filename( png_out + "write_test_string.png" );

        write_view( filename
                  , create_mandel_view( 320, 240
                                      , rgb8_pixel_t( 0,   0, 255 )
                                      , rgb8_pixel_t( 0, 255,   0 )
                                      )
                  , tag_t()
                  );
    }

    {
        string filename( png_out + "write_test_ofstream.png" );

        ofstream out( filename.c_str(), ios::out | ios::binary );

        write_view( out
                  , create_mandel_view( 320, 240
                                      , rgb8_pixel_t( 0,   0, 255 )
                                      , rgb8_pixel_t( 0, 255,   0 )
                                      )
                  , tag_t()
                  );
    }

    {
        string filename( png_out + "write_test_file.png" );

        FILE* file = fopen( filename.c_str(), "wb" );
        
        write_view( file
                  , create_mandel_view( 320, 240
                                      , rgb8_pixel_t( 0,   0, 255 )
                                      , rgb8_pixel_t( 0, 255,   0 )
                                      )
                  , tag_t()
                  );
    }

    {
        string filename( png_out + "write_test_info.png" );
        FILE* file = fopen( filename.c_str(), "wb" );

        image_write_info< png_tag > info;

        write_view( file
                  , create_mandel_view( 320, 240
                                      , rgb8_pixel_t( 0,   0, 255 )
                                      , rgb8_pixel_t( 0, 255,   0 )
                                      )
                  , info
                  );
    }
}

BOOST_AUTO_TEST_CASE( stream_test )
{
    // 1. Read an image.
    ifstream in( png_filename.c_str(), ios::binary );

    rgba8_image_t img;
    read_image( in, img, tag_t() );

    // 2. Write image to in-memory buffer.
    stringstream out_buffer( ios_base::in | ios_base::out | ios_base::binary );
    write_view( out_buffer, view( img ), tag_t() );

    // 3. Copy in-memory buffer to another.
    stringstream in_buffer( ios_base::in | ios_base::out | ios_base::binary );
    in_buffer << out_buffer.rdbuf();

    // 4. Read in-memory buffer to gil image
    rgba8_image_t dst;
    read_image( in_buffer, dst, tag_t() );

    // 5. Write out image.
    string filename( png_out + "stream_test.png" );
    ofstream out( filename.c_str(), ios_base::binary );
    write_view( out, view( dst ), tag_t() );
}

BOOST_AUTO_TEST_CASE( stream_test_2 )
{
    filebuf in_buf;
    if( !in_buf.open( png_filename.c_str(), ios::in | ios::binary ) )
    {
        BOOST_CHECK( false );
    }

    istream in( &in_buf );

    rgba8_image_t img;
    read_image( in, img, tag_t() );
}

BOOST_AUTO_TEST_CASE( subimage_test )
{
    run_subimage_test< rgba8_image_t, tag_t >( png_filename
                                             , point_t(  0,  0 )
                                             , point_t( 50, 50 )
                                             );


    run_subimage_test< rgba8_image_t, tag_t >( png_filename
                                             , point_t( 135, 95 )
                                             , point_t(  50, 50 )
                                             );
}

BOOST_AUTO_TEST_CASE( dynamic_image_test )
{
    typedef mpl::vector< gray8_image_t
                       , gray16_image_t
                       , rgb8_image_t
                       , rgba8_image_t
                       > my_img_types;


    any_image< my_img_types > runtime_image;

    read_image( png_filename.c_str()
              , runtime_image
              , tag_t()
              );

    write_view( png_out + "dynamic_image_test.png"
              , view( runtime_image )
              , tag_t()
              );
}

BOOST_AUTO_TEST_SUITE_END()
