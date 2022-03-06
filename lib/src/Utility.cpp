#include <ymir/Utility.h>
#include <ctime>

namespace ymir {

double measureRuntime(std::function<void()> Func) {
  auto Start = std::clock();
  Func();
  auto End = std::clock();
  return double(End - Start) / CLOCKS_PER_SEC * 1000.0;
}

}