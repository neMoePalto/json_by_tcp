#pragma once

//#ifndef DATAHANDLER_H
//#error "Do not include this file directly! Use datahandler.h!"
//#endif

#include "all_struct_parser/structparserlight.h"
#include "datahandler.h"

#include <QDebug>


struct Struct001;
struct DataOne;
struct DataTwo;

template<typename T, typename PFamily>
void DataHandler::handle(const std::map<unsigned short, std::shared_ptr<PFamily>>* dataParsers) {
  // Чтобы не заводить дополнительных шаблонных функций (в клиентском коде),
  // предусмотрена возможность передачи в данный метод nullptr на коллекцию парсеров.
  if (dataParsers == nullptr) {
    return;
  }

  for (const auto &obj : *dataParsers) {
    auto parserLight = std::dynamic_pointer_cast<StructParserLight<T>>(obj.second);
    if (parserLight != nullptr) {
      auto vec = parserLight->getData();
      processData<T>(vec);
      return;
    }
  }
}


template<typename T>
void DataHandler::processData(std::vector<T>& data) {
  for (const auto &obj : data) {
    processObj(obj);
  }

  data.clear();
}


// Необходимо дописывать специализации для всех типов данных:
template<>
void DataHandler::processObj(const Struct001& /*obj*/) {
  qDebug() << "specialization for Struct001";
}


template<>
void DataHandler::processObj(const DataOne& /*obj*/) {
  qDebug() << "\tspecialization for DataOne";
}


template<>
void DataHandler::processObj(const DataTwo& /*obj*/) {
  qDebug() << "specialization for DataTwo";
}
