#ifndef YMIR_ALGORITHM_LINE_OF_SIGHT_HPP
#define YMIR_ALGORITHM_LINE_OF_SIGHT_HPP

#include <cmath>
#include <ymir/Map.hpp>
#include <iostream>

namespace ymir::Algorithm {

template <typename SeePosPred, typename TileCord>
void traverseLOS(SeePosPred SeePos, ymir::Point2d<TileCord> Start,
                 unsigned int Range, double RadStep = 0.03,
                 double StepSize = 0.5) {
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

} // namespace ymir::Algorithm

#endif // #ifndef YMIR_ALGORITHM_LINE_OF_SIGHT_HPP