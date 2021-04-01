#include <cmath>
#include <iostream>
#include <unistd.h>
#include <ymir/CallularAutomata.hpp>
#include <ymir/Map.hpp>
#include <ymir/Noise.hpp>

template <typename TileType, typename U>
void generate_caves(ymir::Map<TileType, U> &M, TileType Ground, TileType Wall,
                    ymir::Point2d<U> Offset) {
  ymir::UniformRndEng<ymir::WyHashRndEng, float> RE(0, 1);

  // Make entire map walls
  M.fillRect(Wall);

  // Generate initial ground tiles
  const float GroundChance = 0.65f;
  ymir::fillRectSeedRandom(M, Ground, GroundChance, RE,
                           ymir::Rect2d<U>{{0, 0}, {0, 0}}, Offset);

  // Run replacement
  const std::size_t ReplaceThres = 4, Iterations = 6;
  for (std::size_t Idx = 0; Idx < Iterations; Idx++) {
    ymir::celat::replace(M, Ground, Wall, ReplaceThres);
  }

  // Smooth generation
  const std::size_t SmoothThres = 5;
  ymir::celat::generate(M, Ground, SmoothThres);

  // Kill isolated ground
  ymir::celat::generate(M, Wall, 8);
}

constexpr float deg2rad(float Degree) { return Degree / 360 * 2 * M_PI; }

int main(int Argc, char *Argv[]) {
  int OffsetX = 0, OffsetY = 0;
  if (Argc == 3) {
    OffsetX = std::stoi(Argv[1]);
    OffsetY = std::stoi(Argv[2]);
  }
  ymir::Map<char> Map(80, 24);

  for (float Degree = 0; Degree < 360; Degree += 1.0) {
    int PosY = sin(deg2rad(Degree)) * 50;
    int PosX = cos(deg2rad(Degree)) * 50;
    //generate_caves(Map, ' ', '#', {OffsetX + PosX, OffsetY + PosY});
    (void)(PosX + OffsetX);
    (void)(PosY + OffsetY);
    std::cout << "\e[1;1H\e[2J" << Map;
    usleep(100 * 1000);
  }
}
