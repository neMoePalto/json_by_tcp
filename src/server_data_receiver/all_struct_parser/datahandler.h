#pragma once

#include <map>
#include <vector>
#include <memory>


//template<typename T> class StructParserLight;

class DataHandler {
public:
  template<typename T, typename PFamily>
  void handle(const std::map<unsigned short, std::shared_ptr<PFamily>>* dataParsers);

private:
  template<typename T>
  void processData(std::vector<T>& data);

  template<typename T>
  void processObj(const T& obj); // {}
};

#include "datahandler_imlp.h"
