#pragma once

#include <vector>

#include "abstractp.h"


// Примеры структур:
struct DataOne {
  char a;
  int  b;
};

struct DataTwo {
  int   a;
  int   b;
  short c;
};


template<typename T>
class StructParserLight : public AbstractP {
public:
  StructParserLight(unsigned short num); // Если номер нужен

  void initStruct(std::string& data, unsigned int len) override;
  void clearCollection() noexcept override;
  // Простой вариант обработки объектов типа T:
  // реализовать в каждой специализации класса этот метод.
  // Недостаток такого подхода: и сериализация объектов,
  // и их обработка происходят в одном классе:
  //    void useData() override;
  std::vector<T> getData();

private:
  std::vector<T> _structs;
};

#include "structparserlight_impl.h"
