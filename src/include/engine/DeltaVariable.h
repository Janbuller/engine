#pragma once

#include <array>
#include <cstddef>
namespace engine {

  // Template class, which allows for getting the change in an array
  // of variables.

  // It takes and array of variables and returns the difference
  // between those variables and the ones from the last time it was
  // called, then it stores the given variables for next time.

  template <typename T, std::size_t size>
  class DeltaVariable {
  private:
    // Array of variables from last vall.
    std::array<T, size> lastVars;
  public:

    // Initialize array of last variables in constructor.
    DeltaVariable(std::array<T, size> start) {
      lastVars = start;
    }

    std::array<T, size> GetDelta(std::array<T, size> absolute) {
      std::array<T, size> delta;
      for(int i = 0; i < size; i++) {
	delta[i] = absolute[i] - lastVars[i];
	lastVars[i] = absolute[i];
      }
      return delta;
    }

    std::size_t GetSize() {
      return size;
    }
  };

}
