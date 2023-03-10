#include "averagetime.h"

#include <sys/time.h>

#include <QDebug>


void AverageTime::fixStartTime() {
  //    qWarning() << "B: fixStartTime()";
  if (_currentPair.a == 0) {
    _currentPair.a = getMicrosecondsTime();
  }
}


void AverageTime::fixEndTime() {
  //    qWarning() << "E:   fixEndTime()";
  _currentPair.b = getMicrosecondsTime();
  if (_currentPair.a != 0) {
    _vec.push_back(_currentPair);
  }

  _currentPair = {0, 0};
}


double AverageTime::getAverage() {
  if (_vec.empty()) {
    return 0; // Не хватает исходных данных для проведения вычислений
  }

  double average = 0;
  for (const auto& pair : _vec) {
    average = average + (pair.b - pair.a);
  }
  average = average / _vec.size();

  _vec.clear();

//      return average * 1'000'000;
  return average * 1e-6;
}


long AverageTime::getMicrosecondsTime() {
  struct timeval currentTime;
  gettimeofday(&currentTime, nullptr);

  return currentTime.tv_sec * 1'000'000 + currentTime.tv_usec;
}
