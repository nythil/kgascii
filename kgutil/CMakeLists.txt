PROJECT(LIBKGUTIL)

SET(kgutil_SRCS
    resample/filter/bell.hpp
    resample/filter/box.hpp
    resample/filter/bspline.hpp
    resample/filter/lanczos.hpp
    resample/filter/triangle.hpp
    resample/resampler.hpp
    resample.hpp
    enum_wrapper.hpp 
    image_io.hpp
    srgb.hpp
    task_queue.hpp
)

INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Boost_GIL_2_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${JPEG_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIR})

ADD_LIBRARY(kgutil STATIC ${kgutil_SRCS})
SET_TARGET_PROPERTIES(kgutil PROPERTIES LINKER_LANGUAGE CXX)

