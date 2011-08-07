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

#ifndef KGASCII_SRGB_HPP
#define KGASCII_SRGB_HPP

#include <boost/gil/gil_all.hpp>

namespace boost { namespace gil {

template<typename BaseChannelValue>
struct srgb_channel_value
{
    typedef srgb_channel_value          value_type;
    typedef value_type&                 reference;
    typedef value_type*                 pointer;
    typedef const value_type&           const_reference;
    typedef const value_type*           const_pointer;
    static const bool                   is_mutable = true;

    static value_type min_value() { return (std::numeric_limits<BaseChannelValue>::min)(); }
    static value_type max_value() { return (std::numeric_limits<BaseChannelValue>::max)(); }

    srgb_channel_value() {}
    srgb_channel_value(const srgb_channel_value& c) : value_(c.value_) {}
    srgb_channel_value(BaseChannelValue val) : value_(val) {}
    template<typename Scalar> srgb_channel_value(Scalar v) : value_(v) {}

    srgb_channel_value& operator=(BaseChannelValue v) { value_ = v; return *this; }
    operator BaseChannelValue() const { return value_; }

private:
    BaseChannelValue value_;
};

typedef srgb_channel_value<boost::gil::bits16>  srgb16;
typedef srgb_channel_value<boost::gil::bits16s> srgb16s;
typedef srgb_channel_value<boost::gil::bits32>  srgb32;
typedef srgb_channel_value<boost::gil::bits32s> srgb32s;
typedef srgb_channel_value<boost::gil::bits32f> srgb32f;

#define SRGB_DEFINE_BASE_TYPEDEFS_INTERNAL(T, CS, LAYOUT)                                                       \
    typedef boost::gil::pixel<T, LAYOUT >                                               CS##T##_pixel_t;        \
    typedef const boost::gil::pixel<T, LAYOUT >                                         CS##T##c_pixel_t;       \
    typedef CS##T##_pixel_t&                                                            CS##T##_ref_t;          \
    typedef CS##T##c_pixel_t&                                                           CS##T##c_ref_t;         \
    typedef CS##T##_pixel_t*                                                            CS##T##_ptr_t;          \
    typedef CS##T##c_pixel_t*                                                           CS##T##c_ptr_t;         \
    typedef boost::gil::memory_based_step_iterator<CS##T##_ptr_t>                       CS##T##_step_ptr_t;     \
    typedef boost::gil::memory_based_step_iterator<CS##T##c_ptr_t>                      CS##T##c_step_ptr_t;    \
    typedef boost::gil::memory_based_2d_locator<CS##T##_step_ptr_t >                    CS##T##_loc_t;          \
    typedef boost::gil::memory_based_2d_locator<CS##T##c_step_ptr_t >                   CS##T##c_loc_t;         \
    typedef boost::gil::memory_based_2d_locator<boost::gil::memory_based_step_iterator<CS##T##_step_ptr_t> >  CS##T##_step_loc_t;       \
    typedef boost::gil::memory_based_2d_locator<boost::gil::memory_based_step_iterator<CS##T##c_step_ptr_t> > CS##T##c_step_loc_t;      \
    typedef boost::gil::image_view<CS##T##_loc_t>                                       CS##T##_view_t;         \
    typedef boost::gil::image_view<CS##T##c_loc_t>                                      CS##T##c_view_t;        \
    typedef boost::gil::image_view<CS##T##_step_loc_t>                                  CS##T##_step_view_t;    \
    typedef boost::gil::image_view<CS##T##c_step_loc_t>                                 CS##T##c_step_view_t;   \
    typedef boost::gil::image<CS##T##_pixel_t, false, std::allocator<unsigned char> >   CS##T##_image_t;

#define SRGB_DEFINE_BASE_TYPEDEFS(T, CS)        \
        SRGB_DEFINE_BASE_TYPEDEFS_INTERNAL(srgb##T, CS, boost::gil::CS##_layout_t)

SRGB_DEFINE_BASE_TYPEDEFS(16,  gray)
SRGB_DEFINE_BASE_TYPEDEFS(16s, gray)
SRGB_DEFINE_BASE_TYPEDEFS(32 , gray)
SRGB_DEFINE_BASE_TYPEDEFS(32s, gray)
//SRGB_DEFINE_BASE_TYPEDEFS(32f, gray)

template<typename BaseChannel>
struct channel_converter_unsigned<srgb_channel_value<BaseChannel>, bits32f>
        : public std::unary_function<srgb_channel_value<BaseChannel>, bits32f>
{
    bits32f operator()(srgb_channel_value<BaseChannel> x) const
    {
        bits32f fx = channel_convert<bits32f, BaseChannel>(x);
        bits32f fy;
        if (fx < 0.0031308f) {
            fy = 12.92f * fx;
        } else {
            fy = 1.055f * ::powf(fx, 1.0f / 2.4f) - 0.055f;
        }
        return fy;
    }
};

template <typename BaseChannel>
struct channel_converter_unsigned<bits32f, srgb_channel_value<BaseChannel> >
    : public std::unary_function<bits32f, srgb_channel_value<BaseChannel> >
{
    srgb_channel_value<BaseChannel> operator()(bits32f fx) const
    {
        bits32f fy;
        if (fx < 0.04045f) {
            fy = fx / 12.92f;
        } else {
            fy = ::powf((fx + 0.055f) / 1.055f, 2.4f);
        }
        return channel_convert<BaseChannel, bits32f>(fy);
    }
};

template<typename BaseChannel, typename DstChannelV>
struct channel_converter_unsigned<srgb_channel_value<BaseChannel>, DstChannelV>
        : public std::unary_function<srgb_channel_value<BaseChannel>, DstChannelV>
{
    DstChannelV operator()(srgb_channel_value<BaseChannel> x) const
    {
        return channel_convert<DstChannelV, bits32f>(channel_convert<bits32f, srgb_channel_value<BaseChannel> >(x));
    }
};

template <typename SrcChannelV, typename BaseChannel>
struct channel_converter_unsigned<SrcChannelV, srgb_channel_value<BaseChannel> >
    : public std::unary_function<SrcChannelV, srgb_channel_value<BaseChannel> >
{
    srgb_channel_value<BaseChannel> operator()(SrcChannelV x) const
    {
        return channel_convert<srgb_channel_value<BaseChannel>, bits32f>(channel_convert<bits32f, SrcChannelV>(x));
    }
};

template<typename BaseChannel>
struct channel_converter_unsigned<srgb_channel_value<BaseChannel>, srgb_channel_value<BaseChannel> >
    : public std::unary_function<srgb_channel_value<BaseChannel>, srgb_channel_value<BaseChannel> >
{
    srgb_channel_value<BaseChannel> operator()(srgb_channel_value<BaseChannel> x) const { return x; }
};

namespace detail {

template <> struct channel_convert_to_unsigned<srgb16s> : public std::unary_function<srgb16s,srgb16> {
    typedef srgb16 type;
    type operator()(srgb16s  val) const { return val+32768; }
};

template <> struct channel_convert_to_unsigned<srgb32s> : public std::unary_function<srgb32s,srgb32> {
    typedef srgb32 type;
    type operator()(srgb32s  val) const { return static_cast<bits32>(val+(1<<31)); }
};

template <> struct channel_convert_from_unsigned<srgb16s> : public std::unary_function<srgb16,srgb16s> {
    typedef srgb16s type;
    type operator()(srgb16 val) const { return val-32768; }
};

template <> struct channel_convert_from_unsigned<srgb32s> : public std::unary_function<srgb32,srgb32s> {
    typedef srgb32s type;
    type operator()(srgb32 x) const { return static_cast<bits32s>(x-(1<<31)); }
};

} } // namespace gil::detail

template<typename BaseChannelValue>
struct is_integral<gil::srgb_channel_value<BaseChannelValue> > : public is_integral<BaseChannelValue> {};

} // namespace boost


#endif // KGASCII_SRGB_HPP
