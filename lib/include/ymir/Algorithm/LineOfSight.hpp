#ifndef YMIR_ALGORITHM_LINE_OF_SIGHT_HPP
#define YMIR_ALGORITHM_LINE_OF_SIGHT_HPP

#include <cmath>
#include <iostream>
#include <ymir/Map.hpp>

namespace ymir::Algorithm {

template <typename SeePosPred, typename TileCord>
void traverseLOS(SeePosPred SeePos, Point2d<TileCord> Start, unsigned Range,
                 double StepSize = 0.5, double RadStep = 0.03) {
  // Adapt step size based on range
  StepSize = StepSize / Range;

  for (double Rad = 0; Rad < M_PI * 2; Rad += RadStep) {
    // Compute distance in each axis to traverse
    const double Dx = std::cos(Rad) * Range;
    const double Dy = std::sin(Rad) * Range;

    // Step towards target position
    double RelativeDist = 0.0;
    while (RelativeDist < 1.0) {
      TileCord Tx = TileCord(double(Start.X) + 0.5 + RelativeDist * Dx);
      TileCord Ty = TileCord(double(Start.Y) + 0.5 + RelativeDist * Dy);

      ymir::Point2d<TileCord> Pos{Tx, Ty};
      if (!SeePos(Pos)) {
        break;
      }
      RelativeDist += StepSize;
    }
  }
}
template <typename IsLOSBlockedPred, typename TileCord>
bool isInLOS(IsLOSBlockedPred IsLOSBlocked, Point2d<TileCord> Start,
             Point2d<TileCord> Target, unsigned Range, double StepSize = 0.5) {
  try {
    // FIXME do not check entire line of sight,, i.e. 360°, instead
    // only for a specific cone in direction of the target, we need to
    // do this in order to align LOS to target with the entire LOS
    traverseLOS([IsLOSBlocked, Target](auto Pos) {
      if (Target == Pos) {
        throw Target;
      }
      return !IsLOSBlocked(Pos);
    }, Start, Range, StepSize);
  } catch(Point2d<TileCord>) {
    return true;
  }
  return false;
}

} // namespace ymir::Algorithm

#endif // #ifndef YMIR_ALGORITHM_LINE_OF_SIGHT_HPP