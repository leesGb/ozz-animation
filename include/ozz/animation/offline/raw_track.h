//----------------------------------------------------------------------------//
//                                                                            //
// ozz-animation is hosted at http://github.com/guillaumeblanc/ozz-animation  //
// and distributed under the MIT License (MIT).                               //
//                                                                            //
// Copyright (c) 2015 Guillaume Blanc                                         //
//                                                                            //
// Permission is hereby granted, free of charge, to any person obtaining a    //
// copy of this software and associated documentation files (the "Software"), //
// to deal in the Software without restriction, including without limitation  //
// the rights to use, copy, modify, merge, publish, distribute, sublicense,   //
// and/or sell copies of the Software, and to permit persons to whom the      //
// Software is furnished to do so, subject to the following conditions:       //
//                                                                            //
// The above copyright notice and this permission notice shall be included in //
// all copies or substantial portions of the Software.                        //
//                                                                            //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    //
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    //
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        //
// DEALINGS IN THE SOFTWARE.                                                  //
//                                                                            //
//----------------------------------------------------------------------------//

#ifndef OZZ_OZZ_ANIMATION_OFFLINE_RAW_TRACK_H_
#define OZZ_OZZ_ANIMATION_OFFLINE_RAW_TRACK_H_

#include "ozz/base/containers/vector.h"
#include "ozz/base/io/archive_traits.h"

// TODO see if it's the right place
#include "ozz/base/maths/quaternion.h"
#include "ozz/base/maths/vec_float.h"

namespace ozz {
namespace animation {
namespace offline {

// Interpolation mode.
struct RawTrackInterpolation {
  enum Value {
    kStep,    // All values following this key, up to the next key, are equal.
    kLinear,  // All value between this key and the next are linearly
              // interpolated.
  };
};

// Keyframe data structure.
template <typename _ValueType>
struct RawTrackKeyframe {
  typedef _ValueType ValueType;
  RawTrackInterpolation::Value interpolation;
  float time;
  ValueType value;
};

namespace internal {

// Offline float animation track type.
// This data type is not intended to be used in run time. It is used to
// define the offline float curve/track object that can be converted to the
// runtime
// channel using the FlatTrackBuilder.
// This animation structure exposes a single float sequence of keyframes.
// Keyframes are defined with a time, a float value and an interpolation mode
// (impact the range from the keyframe to the next). Float track structure is
// then a
// sorted vector of keyframes. A track has no duration, keyframes time range
// must be between 0 and 1.
// Finally the RawFloatTrack structure exposes Validate() function to check that
// it is valid, meaning that all the following rules are respected:
//  1. Keyframes' time are sorted in a strict ascending order.
//  2. Keyframes' time are all within [0,1] range.
//  3. Successive keyframes' time must be separated by at least
// std::numeric_limits<float>::epsilon() (around 1e-7).
// RawFloatTrack that would fail this validation will fail to be converted by
// the
// RawFloatTrackBuilder.
template <typename _ValueType>
struct RawTrack {
  typedef _ValueType ValueType;
  typedef RawTrackKeyframe<ValueType> Keyframe;

  // Constructs a valid RawFloatTrack.
  RawTrack();

  // Deallocates track.
  ~RawTrack();

  // Validates that all the following rules are respected:
  //  1. Keyframes' time are sorted in a strict ascending order.
  //  2. Keyframes' time are all within [0,1] range.
  //  3. Successive keyframes' time must be separated by at least
  // std::numeric_limits<float>::epsilon (1e-5).
  bool Validate() const;

  // Uses intrusive serialization option, as a way to factorize code.
  // Version and Tag should still be defined for each specialization.
  void Save(io::OArchive& _archive) const;
  void Load(io::IArchive& _archive, uint32_t _version);

  // Sequence of keyframes, expected to be sorted.
  typedef typename ozz::Vector<Keyframe>::Std Keyframes;
  Keyframes keyframes;
};
}  // namespace internal

struct RawFloatTrack : public internal::RawTrack<float> {};
struct RawFloat2Track : public internal::RawTrack<math::Float2> {};
struct RawFloat3Track : public internal::RawTrack<math::Float3> {};
struct RawQuaternionTrack : public internal::RawTrack<math::Quaternion> {};
}  // namespace offline
}  // namespace animation

namespace io {
OZZ_IO_TYPE_VERSION(1, animation::offline::RawFloatTrack)
OZZ_IO_TYPE_TAG("ozz-raw_float_track", animation::offline::RawFloatTrack)
OZZ_IO_TYPE_VERSION(1, animation::offline::RawFloat2Track)
OZZ_IO_TYPE_TAG("ozz-raw_float2_track", animation::offline::RawFloat2Track)
OZZ_IO_TYPE_VERSION(1, animation::offline::RawFloat3Track)
OZZ_IO_TYPE_TAG("ozz-raw_float3_track", animation::offline::RawFloat3Track)
OZZ_IO_TYPE_VERSION(1, animation::offline::RawQuaternionTrack)
OZZ_IO_TYPE_TAG("ozz-raw_quatt_track", animation::offline::RawQuaternionTrack)
}  // namespace io
}  // namespace ozz
#endif  // OZZ_OZZ_ANIMATION_OFFLINE_RAW_TRACK_H_