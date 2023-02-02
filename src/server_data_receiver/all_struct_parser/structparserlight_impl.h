#pragma once

//#ifndef STRUCTPARSERLIGHT_H
//#error "Do not include this file directly! Use structparser.h!"
////static_assert (0, "Do not include this file directly! Use structparser.h!");
//#endif

#include "structparserlight.h"

#include <QDebug>

//#include "../shared/structs/struct001.h"


template<typename T>
StructParserLight<T>::StructParserLight(unsigned short num)
  : AbstractP(num) {
}


template<typename T>
void StructParserLight<T>::initStruct(std::string& data, unsigned int len) {
  T myStruct;
  memcpy(&myStruct, data.data(), len);

  data.erase(0, len);
  _structs.push_back(myStruct);
  qDebug() << "sizeof(T)=" << sizeof (T);
}


template<typename T>
void StructParserLight<T>::clearCollection() noexcept {
  _structs.clear();
}

//template<typename T>
//void StructParserLight<T>::useData() {
//  _structs.clear();
//}

//template<>
//void StructParserLight<Struct001>::useData() {
//  qDebug() << "\n";
//  for (auto &obj : _structs) {
//    // Обработка структуры:
//  }
//  _structs.clear();
//}

//template<>
//void StructParserLight<DataOne>::useData() {
//  qDebug() << "Test impl for struct DataOne";
//}


template<typename T>
std::vector<T> StructParserLight<T>::getData() {
  return _structs;
}
