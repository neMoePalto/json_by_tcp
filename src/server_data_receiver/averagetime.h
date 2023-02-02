#pragma once

#include <vector>


class AverageTime {
public:
  void fixStartTime();
  void fixEndTime();
  double getAverage();

private:
  long getMicrosecondsTime();

private:
  struct PairOfValues {
    long a;
    long b;
  };

  std::vector<PairOfValues> _vec;
  PairOfValues              _currentPair;
};
