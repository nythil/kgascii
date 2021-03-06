/********************************************************
 *
 * This test file will test gray tiled tiff writing
 *
 *******************************************************/
//#define BOOST_TEST_MODULE tiff_tiled_miniblack_write_test_11_20_module
#include <boost/test/unit_test.hpp>

#include "tiff_tiled_write_macros.hpp"

BOOST_AUTO_TEST_SUITE( tiff_test )

BOOST_PP_REPEAT_FROM_TO(11, 16, GENERATE_WRITE_TILE_BIT_ALIGNED_MINISBLACK, minisblack )
BOOST_PP_REPEAT_FROM_TO(17, 21, GENERATE_WRITE_TILE_BIT_ALIGNED_MINISBLACK, minisblack )

BOOST_AUTO_TEST_CASE( write_minisblack_tile_and_compare_with_16 )
{
    using namespace std;
    using namespace boost;
    using namespace gil;

    string filename_strip( tiff_in_GM + "tiger-minisblack-strip-16.tif" );

    gray16_image_t img_strip, img_saved;

    read_image( filename_strip, img_strip, tag_t() );

    image_write_info<tag_t> info;

    info._is_tiled = true;
    info._tile_width = info._tile_length = 16;

    write_view( tiff_out + "write_minisblack_tile_and_compare_with_16.tif", view(img_strip), info );
    read_image( tiff_out + "write_minisblack_tile_and_compare_with_16.tif", img_saved, tag_t() );
    BOOST_CHECK_EQUAL( equal_pixels( const_view(img_strip), const_view(img_saved) ), true);
}

BOOST_AUTO_TEST_SUITE_END()
