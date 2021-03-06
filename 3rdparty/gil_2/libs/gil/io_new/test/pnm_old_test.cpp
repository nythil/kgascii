//#define BOOST_TEST_MODULE pnm_old_test_module
#include <boost/test/unit_test.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io_new/pnm_io_old.hpp>

#include "paths.hpp"

using namespace std;
using namespace boost;
using namespace gil;

BOOST_AUTO_TEST_SUITE( pnm_test )

BOOST_AUTO_TEST_CASE( old_read_dimensions_test )
{
    {
        point2< ptrdiff_t > dim = pnm_read_dimensions( pnm_filename );

        BOOST_CHECK_EQUAL( dim.x, 256 );
        BOOST_CHECK_EQUAL( dim.y, 256 );
    }
}

BOOST_AUTO_TEST_CASE( old_read_image_test )
{
    {
        rgb8_image_t img;
        pnm_read_image( pnm_filename, img );

        BOOST_CHECK_EQUAL( img.width() , 256 );
        BOOST_CHECK_EQUAL( img.height(), 256 );
    }
}

BOOST_AUTO_TEST_CASE( old_read_and_convert_image_test )
{
    {
        rgb8_image_t img;
        pnm_read_and_convert_image( pnm_filename, img );

        BOOST_CHECK_EQUAL( img.width() , 256 );
        BOOST_CHECK_EQUAL( img.height(), 256 );
    }
}

BOOST_AUTO_TEST_CASE( old_read_view_test )
{
    {
        rgb8_image_t img( 256, 256 );
        pnm_read_view( pnm_filename, view( img ) );
    }
}

BOOST_AUTO_TEST_CASE( old_read_and_convert_view_test )
{
    {
        rgb8_image_t img( 256, 256 );
        pnm_read_and_convert_view( pnm_filename, view( img ) );
    }
}

BOOST_AUTO_TEST_CASE( old_write_view_test )
{
    {
        string filename( pnm_out + "test5.pnm" );

        gray8_image_t img( 256, 256);
        pnm_write_view( filename, view( img ) );
    }
}

BOOST_AUTO_TEST_CASE( old_dynamic_image_test )
{
    typedef mpl::vector< gray8_image_t
                       , gray16_image_t
                       , rgb8_image_t
                       , gil::detail::gray1_image_t
                       > my_img_types;


    any_image< my_img_types > runtime_image;

    pnm_read_image( pnm_filename.c_str()
                  , runtime_image
                  );

    pnm_write_view( pnm_out + "old_dynamic_image_test.pnm"
                  , view( runtime_image )
                  );
}

BOOST_AUTO_TEST_SUITE_END()
