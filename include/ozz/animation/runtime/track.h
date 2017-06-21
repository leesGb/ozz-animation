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

#ifndef OZZ_OZZ_ANIMATION_RUNTIME_TRACK_H_
#define OZZ_OZZ_ANIMATION_RUNTIME_TRACK_H_

#include "ozz/base/io/archive_traits.h"
#include "ozz/base/platform.h"

#include "ozz/base/maths/quaternion.h"
#include "ozz/base/maths/vec_float.h"

namespace ozz {
namespace animation {

// Forward declares the TrackBuilder, used to instantiate a Track.
namespace offline {
class TrackBuilder;
}

namespace internal {
// Runtime float track data structure.
template <typename _ValueType>
class Track {
 public:
  typedef _ValueType ValueType;

  Track();
  ~Track();

  Range<const float> times() const { return times_; }
  Range<const _ValueType> values() const { return values_; }
  Range<const uint8_t> steps() const { return steps_; }

  // Get the estimated track's size in bytes.
  size_t size() const;

  // Serialization functions.
  // Should not be called directly but through io::Archive << and >> operators.
  void Save(ozz::io::OArchive& _archive) const;
  void Load(ozz::io::IArchive& _archive, uint32_t _version);

 private:
  // TrackBuilder class is allowed to allocate an Animation.
  friend class offline::TrackBuilder;

  // Internal destruction function.
  void Allocate(size_t _keys_count);
  void Deallocate();

  Range<float> times_;
  Range<_ValueType> values_;
  Range<uint8_t> steps_;
};

// Definition of operations policies per track value type.
template <typename _ValueType>
struct TrackPolicy {
  inline static _ValueType Lerp(const _ValueType& _a, const _ValueType& _b,
                                float _alpha) {
    return math::Lerp(_a, _b, _alpha);
  }
  inline static _ValueType identity() { return _ValueType(0.f); }
};
// Specialization for quaternions policy.
template <>
inline math::Quaternion TrackPolicy<math::Quaternion>::Lerp(
    const math::Quaternion& _a, const math::Quaternion& _b, float _alpha) {
  // Uses NLerp to favor speed. This same function is used when optimizing the
  // curve (key frame reduction), so "constant speed" interpolation can still be
  // approximated with a lower tolerance value if it matters.
  return math::NLerp(_a, _b, _alpha);
}
template <>
inline math::Quaternion TrackPolicy<math::Quaternion>::identity() {
  return math::Quaternion::identity();
}
}  // namespace internal

// Runtime float track data structure.
class FloatTrack : public internal::Track<float> {};
class Float2Track : public internal::Track<math::Float2> {};
class Float3Track : public internal::Track<math::Float3> {};
class QuaternionTrack : public internal::Track<math::Quaternion> {};

}  // namespace animation
namespace io {
OZZ_IO_TYPE_VERSION(1, animation::FloatTrack)
OZZ_IO_TYPE_TAG("ozz-float_track", animation::FloatTrack)

// Should not be called directly but through io::Archive << and >> operators.
template <>
struct Extern<animation::FloatTrack> {
  static void Save(OArchive& _archive, const animation::FloatTrack* _tracks,
                   size_t _count);
  static void Load(IArchive& _archive, animation::FloatTrack* _tracks,
                   size_t _count, uint32_t _version);
};
}  // namespace io
}  // namespace ozz
#endif  // OZZ_OZZ_ANIMATION_RUNTIME_TRACK_H_