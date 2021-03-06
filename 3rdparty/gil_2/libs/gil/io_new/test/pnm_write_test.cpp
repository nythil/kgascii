//#define BOOST_TEST_MODULE pnm_write_test_module
#include <boost/test/unit_test.hpp>

#include <boost/gil/extension/io_new/pnm_all.hpp>

#include "mandel_view.hpp"
#include "paths.hpp"

using namespace std;
using namespace boost::gil;

typedef pnm_tag tag_t;

BOOST_AUTO_TEST_SUITE( pnm_test )

BOOST_AUTO_TEST_CASE( write_test )
{
    mandel_view< rgb8_pixel_t >::type v = create_mandel_view( 200, 200
                                                            , rgb8_pixel_t( 0,   0, 255 )
                                                            , rgb8_pixel_t( 0, 255,   0 )
                                                            );

    // test writing all supported image types
    {
        typedef bit_aligned_image1_type< 1, gray_layout_t >::type gray1_image_t;
        gray1_image_t dst( 200, 200 );

        copy_and_convert_pixels( v, view( dst ));

        write_view( pnm_out + "p4_write_test.pnm"
                  , view( dst )
                  , pnm_tag()
                  );
    }

    {
        gray8_image_t dst( 200, 200 );

        copy_and_convert_pixels( v, view( dst ));

        write_view( pnm_out + "p5_write_test.pnm"
                  , view( dst )
                  , pnm_tag()
                  );
    }

    {
        write_view( pnm_out + "p6_write_test.pnm"
                  , v
                  , pnm_tag()
                  );
    }
}

BOOST_AUTO_TEST_SUITE_END()
