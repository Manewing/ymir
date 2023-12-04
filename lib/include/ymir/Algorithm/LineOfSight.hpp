#ifndef YMIR_ALGORITHM_LINE_OF_SIGHT_HPP
#define YMIR_ALGORITHM_LINE_OF_SIGHT_HPP

#include <cmath>
#include <iostream>
#include <ymir/Map.hpp>

namespace ymir::Algorithm {

template <typename TileCord, typename SeePosPred, typename IsPosBlockedPred>
void rayCastDDA(const SeePosPred &SeePos, const IsPosBlockedPred &IsPosBlocked,
                double Range, Point2d<double> Start, Point2d<double> Target) {
  // Based on DDA from:
  //  https://lodev.org/cgtutor/raycasting.html

  // Compute the ray direction
  auto RayDir = (Target - Start).normalized();

  // Compute the ray delta distance, i.e. the length of the ray from one x or y
  // side to the next x or y side
  Point2d<double> DeltaDist;
  DeltaDist.X = RayDir.X == 0 ? 1e30 : std::abs(1.0 / RayDir.X);
  DeltaDist.Y = RayDir.Y == 0 ? 1e30 : std::abs(1.0 / RayDir.Y);
  DeltaDist.X =
      RayDir.X == 0
          ? 1e30
          : std::sqrt(1.0 + (RayDir.Y * RayDir.Y) / (RayDir.X * RayDir.X));
  DeltaDist.Y =
      RayDir.Y == 0
          ? 1e30
          : std::sqrt(1.0 + (RayDir.X * RayDir.X) / (RayDir.Y * RayDir.Y));

  // Compute the step direction and initial side distance
  Point2d<TileCord> MapPos = Start.template to<TileCord>();
  Point2d<TileCord> StepDist;
  Point2d<double> SideDist;
  if (RayDir.X < 0) {
    StepDist.X = -1;
    SideDist.X = (Start.X - MapPos.X) * DeltaDist.X;
  } else {
    StepDist.X = 1;
    SideDist.X = (MapPos.X + 1.0 - Start.X) * DeltaDist.X;
  }
  if (RayDir.Y < 0) {
    StepDist.Y = -1;
    SideDist.Y = (Start.Y - MapPos.Y) * DeltaDist.Y;
  } else {
    StepDist.Y = 1;
    SideDist.Y = (MapPos.Y + 1.0 - Start.Y) * DeltaDist.Y;
  }

  bool Hit = false;
  while (!Hit && (SideDist.X < Range || SideDist.Y < Range)) {
    // Move to the next tile depending on the side distance
    if (SideDist.X < SideDist.Y) {
      SideDist.X += DeltaDist.X;
      MapPos.X += StepDist.X;
    } else {
      SideDist.Y += DeltaDist.Y;
      MapPos.Y += StepDist.Y;
    }

    SeePos(MapPos);
    Hit = IsPosBlocked(MapPos);
  }
}

template <typename TileCord, typename SeePosPred, typename IsPosBlockedPred>
void shadowCastingOctant(TileCord Octant, SeePosPred SeePos,
                         const IsPosBlockedPred &IsPosBlocked,
                         const Point2d<TileCord> Origin, const double Range,
                         TileCord X, Point2d<TileCord> Top,
                         Point2d<TileCord> Bottom) {
  // Based on shadow casting from:
  //  http://www.adammil.net/blog/v125_roguelike_vision_algorithms.html
  static constexpr int Multipliers[4][8] = {
      {1, 0, 0, -1, -1, 0, 0, 1},
      {0, 1, -1, 0, 0, -1, 1, 0},
      {0, 1, 1, 0, 0, -1, -1, 0},
      {1, 0, 0, 1, -1, 0, 0, -1},
  };
  enum class BlockedState { NotApplicable, Blocked, Clear };

  for (; X <= (TileCord)Range; X++) {
    // Compute the Y coordinates where the top vector leaves the column (on the
    // right) and where the bottom vector enters the column (on the left). this
    // equals (x+0.5)*top+0.5 and (x-0.5)*bottom+0.5 respectively, which can be
    // computed like (x+0.5)*top+0.5 = (2(x+0.5)*top+1)/2 = ((2x+1)*top+1)/2 to
    // avoid floating point math
    int StartY = X;
    if (Top.X != 1) {
      StartY = ((X * 2 + 1) * Top.Y + Top.X - 1) / (Top.X * 2);
    }
    int EndY = 0;
    if (Bottom.Y != 0) {
      EndY = ((X * 2 - 1) * Bottom.Y + Bottom.X) / (Bottom.X * 2);
    }

    auto WasBlocked = BlockedState::NotApplicable;
    for (int Y = StartY; Y >= EndY; Y--) {
      auto SP = Origin;
      SP.X += Multipliers[0][Octant] * X + Multipliers[1][Octant] * Y;
      SP.Y += Multipliers[2][Octant] * X + Multipliers[3][Octant] * Y;

      const bool InRange = Point2d<double>(X, Y).length() <= Range;
      if (InRange) {
        SeePos(SP);
      }

      if (X == Range) {
        continue;
      }
      if (!InRange || IsPosBlocked(SP)) {
        if (WasBlocked == BlockedState::Clear) {
          // If we found a transition from clear to opaque, this sector is done
          // in this column, so adjust the bottom vector upwards and continue
          // processing it in the next column. (x*2-1, y*2+1) is a vector to
          // the top-left of the opaque tile
          auto NewBottom = Point2d<TileCord>(X * 2 - 1, Y * 2 + 1);
          if (!InRange || Y == EndY) {
            Bottom = NewBottom;
            break;
          }
          shadowCastingOctant(Octant, SeePos, IsPosBlocked, Origin, Range,
                              X + 1, Top, NewBottom);
        }
        WasBlocked = BlockedState::Blocked;
        continue;
      }

      // Adjust top vector downwards and continue if we found a transition from
      // opaque to clear (x*2+1, y*2+1) is the top-right corner of the clear
      // tile (i.e. the bottom-right of the opaque tile)
      if (WasBlocked == BlockedState::Blocked) {
        Top = {X * 2 + 1, Y * 2 + 1};
      }
      WasBlocked = BlockedState::Clear;
    }

    // if the column ended in a clear tile, continue processing the
    // current sector
    if (WasBlocked != BlockedState::Clear) {
      break;
    }
  }
}

template <typename TileCord, typename SeePosPred, typename IsPosBlockedPred>
void shadowCasting(const SeePosPred &SeePos,
                   const IsPosBlockedPred &IsPosBlocked,
                   Point2d<TileCord> Start, const double Range) {
  SeePos(Start);
  for (TileCord Octant = 0; Octant < 8; Octant++) {
    shadowCastingOctant<int>(Octant, SeePos, IsPosBlocked, Start, Range, 1,
                             {1, 1}, {1, 0});
  }
}

template <typename IsPosBlockedPred, typename TileCord>
bool isInLOS(const IsPosBlockedPred &IsBlocked, Point2d<TileCord> Start,
             Point2d<TileCord> Target, double Range,
             const Point2d<double> Offset = {0.5, 0.5}) {
  bool Ret = false;
  auto SeePos = [&Ret, Target](Point2d<TileCord> Pos) { Ret |= Pos == Target; };
  auto IsPosBlocked = [Target, &IsBlocked](Point2d<TileCord> Pos) -> bool {
    return Pos == Target || IsBlocked(Pos);
  };
  rayCastDDA<TileCord>(SeePos, IsPosBlocked, Range,
                       Start.template to<double>() + Offset,
                       Target.template to<double>() + Offset);
  return Ret;
}

} // namespace ymir::Algorithm

#endif // #ifndef YMIR_ALGORITHM_LINE_OF_SIGHT_HPP