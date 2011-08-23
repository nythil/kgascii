// This file is part of KG::Ascii.
//
// Copyright (C) 2011 Robert Konklewski <nythil@gmail.com>
//
// KG::Ascii is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// KG::Ascii is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with KG::Ascii. If not, see <http://www.gnu.org/licenses/>.

#ifndef KGUTIL_SRGB_HPP
#define KGUTIL_SRGB_HPP

#include <boost/gil/gil_all.hpp>

namespace boost { namespace gil {

template<typename BaseChannelValue>
struct linear_channel_value
{
    typedef linear_channel_value        value_type;
    typedef value_type&                 reference;
    typedef value_type*                 pointer;
    typedef const value_type&           const_reference;
    typedef const value_type*           const_pointer;
    static const bool                   is_mutable = true;

    static value_type min_value() { return (std::numeric_limits<BaseChannelValue>::min)(); }
    static value_type max_value() { return (std::numeric_limits<BaseChannelValue>::max)(); }

    linear_channel_value() {}
    linear_channel_value(const linear_channel_value& c) : value_(c.value_) {}
    linear_channel_value(BaseChannelValue val) : value_(val) {}
    template<typename Scalar> linear_channel_value(Scalar v) : value_(v) {}

    linear_channel_value& operator=(BaseChannelValue v) { value_ = v; return *this; }
    operator BaseChannelValue() const { return value_; }

private:
    BaseChannelValue value_;
};

typedef linear_channel_value<boost::gil::bits16>  lin16;
typedef linear_channel_value<boost::gil::bits16s> lin16s;
typedef linear_channel_value<boost::gil::bits32>  lin32;
typedef linear_channel_value<boost::gil::bits32s> lin32s;
typedef linear_channel_value<boost::gil::bits32f> lin32f;

#define LINEAR_DEFINE_BASE_TYPEDEFS_INTERNAL(T, TPRE, LAYOUT)                                                   \
    typedef boost::gil::pixel<T, LAYOUT >                                               TPRE##_pixel_t;         \
    typedef const boost::gil::pixel<T, LAYOUT >                                         TPRE##c_pixel_t;        \
    typedef TPRE##_pixel_t&                                                             TPRE##_ref_t;           \
    typedef TPRE##c_pixel_t&                                                            TPRE##c_ref_t;          \
    typedef TPRE##_pixel_t*                                                             TPRE##_ptr_t;           \
    typedef TPRE##c_pixel_t*                                                            TPRE##c_ptr_t;          \
    typedef boost::gil::memory_based_step_iterator<TPRE##_ptr_t>                        TPRE##_step_ptr_t;      \
    typedef boost::gil::memory_based_step_iterator<TPRE##c_ptr_t>                       TPRE##c_step_ptr_t;     \
    typedef boost::gil::memory_based_2d_locator<TPRE##_step_ptr_t >                     TPRE##_loc_t;           \
    typedef boost::gil::memory_based_2d_locator<TPRE##c_step_ptr_t >                    TPRE##c_loc_t;          \
    typedef boost::gil::memory_based_2d_locator<boost::gil::memory_based_step_iterator<TPRE##_step_ptr_t> >     TPRE##_step_loc_t;      \
    typedef boost::gil::memory_based_2d_locator<boost::gil::memory_based_step_iterator<TPRE##c_step_ptr_t> >    TPRE##c_step_loc_t;     \
    typedef boost::gil::image_view<TPRE##_loc_t>                                        TPRE##_view_t;          \
    typedef boost::gil::image_view<TPRE##c_loc_t>                                       TPRE##c_view_t;         \
    typedef boost::gil::image_view<TPRE##_step_loc_t>                                   TPRE##_step_view_t;     \
    typedef boost::gil::image_view<TPRE##c_step_loc_t>                                  TPRE##c_step_view_t;    \
    typedef boost::gil::image<TPRE##_pixel_t, false, std::allocator<unsigned char> >    TPRE##_image_t;

#define LINEAR_DEFINE_BASE_TYPEDEFS(T, CS)        \
        LINEAR_DEFINE_BASE_TYPEDEFS_INTERNAL(lin##T, CS##_lin##T, boost::gil::CS##_layout_t)

LINEAR_DEFINE_BASE_TYPEDEFS(16,  gray)
LINEAR_DEFINE_BASE_TYPEDEFS(16s, gray)
LINEAR_DEFINE_BASE_TYPEDEFS(32,  gray)
LINEAR_DEFINE_BASE_TYPEDEFS(32s, gray)
LINEAR_DEFINE_BASE_TYPEDEFS(32f, gray)
LINEAR_DEFINE_BASE_TYPEDEFS(16,  bgr)
LINEAR_DEFINE_BASE_TYPEDEFS(16s, bgr)
LINEAR_DEFINE_BASE_TYPEDEFS(32 , bgr)
LINEAR_DEFINE_BASE_TYPEDEFS(32s, bgr)
LINEAR_DEFINE_BASE_TYPEDEFS(32f, bgr)
LINEAR_DEFINE_BASE_TYPEDEFS(16,  argb)
LINEAR_DEFINE_BASE_TYPEDEFS(16s, argb)
LINEAR_DEFINE_BASE_TYPEDEFS(32,  argb)
LINEAR_DEFINE_BASE_TYPEDEFS(32s, argb)
LINEAR_DEFINE_BASE_TYPEDEFS(32f, argb)
LINEAR_DEFINE_BASE_TYPEDEFS(16,  abgr)
LINEAR_DEFINE_BASE_TYPEDEFS(16s, abgr)
LINEAR_DEFINE_BASE_TYPEDEFS(32 , abgr)
LINEAR_DEFINE_BASE_TYPEDEFS(32s, abgr)
LINEAR_DEFINE_BASE_TYPEDEFS(32f, abgr)
LINEAR_DEFINE_BASE_TYPEDEFS(16,  bgra)
LINEAR_DEFINE_BASE_TYPEDEFS(16s, bgra)
LINEAR_DEFINE_BASE_TYPEDEFS(32 , bgra)
LINEAR_DEFINE_BASE_TYPEDEFS(32s, bgra)
LINEAR_DEFINE_BASE_TYPEDEFS(32f, bgra)
LINEAR_DEFINE_BASE_TYPEDEFS(16,  rgb)
LINEAR_DEFINE_BASE_TYPEDEFS(16s, rgb)
LINEAR_DEFINE_BASE_TYPEDEFS(32 , rgb)
LINEAR_DEFINE_BASE_TYPEDEFS(32s, rgb)
LINEAR_DEFINE_BASE_TYPEDEFS(32f, rgb)
LINEAR_DEFINE_BASE_TYPEDEFS(16,  rgba)
LINEAR_DEFINE_BASE_TYPEDEFS(16s, rgba)
LINEAR_DEFINE_BASE_TYPEDEFS(32 , rgba)
LINEAR_DEFINE_BASE_TYPEDEFS(32s, rgba)
LINEAR_DEFINE_BASE_TYPEDEFS(32f, rgba)
LINEAR_DEFINE_BASE_TYPEDEFS(16,  cmyk)
LINEAR_DEFINE_BASE_TYPEDEFS(16s, cmyk)
LINEAR_DEFINE_BASE_TYPEDEFS(32 , cmyk)
LINEAR_DEFINE_BASE_TYPEDEFS(32s, cmyk)
LINEAR_DEFINE_BASE_TYPEDEFS(32f, cmyk)

template<typename BaseChannel>
struct channel_converter_unsigned<linear_channel_value<BaseChannel>, bits32f>
        : public std::unary_function<linear_channel_value<BaseChannel>, bits32f>
{
    bits32f operator()(linear_channel_value<BaseChannel> x) const
    {
        float fx = channel_convert<bits32f, BaseChannel>(x);
        float fy;
        if (fx <= 0.00304f) {
            fy = 12.92f * fx;
        } else {
            fy = 1.055f * ::powf(fx, 1.0f / 2.4f) - 0.055f;
        }
        return fy;
    }
};

template <typename BaseChannel>
struct channel_converter_unsigned<bits32f, linear_channel_value<BaseChannel> >
    : public std::unary_function<bits32f, linear_channel_value<BaseChannel> >
{
    linear_channel_value<BaseChannel> operator()(bits32f x) const
    {
        float fx = x;
        float fy;
        if (fx <= 0.03928f) {
            fy = fx / 12.92f;
        } else {
            fy = ::powf((fx + 0.055f) / 1.055f, 2.4f);
        }
        return channel_convert<BaseChannel, bits32f>(fy);
    }
};

template<typename BaseChannel, typename DstChannelV>
struct channel_converter_unsigned<linear_channel_value<BaseChannel>, DstChannelV>
        : public std::unary_function<linear_channel_value<BaseChannel>, DstChannelV>
{
    DstChannelV operator()(linear_channel_value<BaseChannel> x) const
    {
        return channel_convert<DstChannelV, bits32f>(channel_convert<bits32f, linear_channel_value<BaseChannel> >(x));
    }
};

template <typename SrcChannelV, typename BaseChannel>
struct channel_converter_unsigned<SrcChannelV, linear_channel_value<BaseChannel> >
    : public std::unary_function<SrcChannelV, linear_channel_value<BaseChannel> >
{
    linear_channel_value<BaseChannel> operator()(SrcChannelV x) const
    {
        return channel_convert<linear_channel_value<BaseChannel>, bits32f>(channel_convert<bits32f, SrcChannelV>(x));
    }
};

template<typename SrcBaseChannel, typename DstBaseChannel>
struct channel_converter_unsigned<linear_channel_value<SrcBaseChannel>, linear_channel_value<DstBaseChannel> >
    : public std::unary_function<linear_channel_value<SrcBaseChannel>, linear_channel_value<DstBaseChannel> >
{
    linear_channel_value<DstBaseChannel> operator()(linear_channel_value<SrcBaseChannel> x) const
    {
        return channel_convert<DstBaseChannel, SrcBaseChannel>(x);
    }
};

template<typename BaseChannel>
struct channel_converter_unsigned<linear_channel_value<BaseChannel>, linear_channel_value<BaseChannel> >
    : public std::unary_function<linear_channel_value<BaseChannel>, linear_channel_value<BaseChannel> >
{
    linear_channel_value<BaseChannel> operator()(linear_channel_value<BaseChannel> x) const { return x; }
};

namespace detail {

template<typename BaseChannel>
struct channel_convert_to_unsigned<linear_channel_value<BaseChannel> >
    : public std::unary_function<
          linear_channel_value<BaseChannel>,
          linear_channel_value<typename channel_convert_to_unsigned<BaseChannel>::type>
          >
{
    typedef linear_channel_value<typename channel_convert_to_unsigned<BaseChannel>::type> type;
    type operator()(linear_channel_value<BaseChannel> val) const
    {
        return channel_convert_to_unsigned<BaseChannel>()(val);
    }
};

template<typename BaseChannel>
struct channel_convert_from_unsigned<linear_channel_value<BaseChannel> >
    : public std::unary_function<
          linear_channel_value<BaseChannel>,
          linear_channel_value<typename channel_convert_from_unsigned<BaseChannel>::type>
          >
{
    typedef linear_channel_value<typename channel_convert_from_unsigned<BaseChannel>::type> type;
    type operator()(linear_channel_value<BaseChannel> val) const
    {
        return channel_convert_from_unsigned<BaseChannel>()(val);
    }
};

} } // namespace gil::detail

template<typename BaseChannelValue>
struct is_integral<gil::linear_channel_value<BaseChannelValue> > : public is_integral<BaseChannelValue> {};

} // namespace boost

namespace KG { namespace Util {

template<typename BaseChannelValue>
struct float_channel_type<boost::gil::linear_channel_value<BaseChannelValue> >
{
    typedef boost::gil::lin32f type;
};

} } // namespace KG::Util

#endif // KGUTIL_SRGB_HPP
