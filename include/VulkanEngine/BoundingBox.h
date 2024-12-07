// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

/// Base class for BoundingBox specializations.
struct BoundingBoxBase {};

/// BoundingBox class.
/// \tparam PositionType The position type to use, e.g Eigen::Vector3f.
template <typename PositionType> struct BoundingBox : public BoundingBoxBase {
  /// The maximum extent of the bounding box.
  PositionType max;

  /// The minimum extent of the bounding box.
  PositionType min;
};

#endif /* BOUNDINGBOX_H */
