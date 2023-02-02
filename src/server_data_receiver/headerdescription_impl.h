#pragma once

//#ifndef HEADERDESCRIPTION_H
//#error "Do not include this file directly! Use headerdescription.h!"
//#endif

#include <algorithm>

#include <netinet/in.h>

#include <QDebug>

#include "headerdescription.h"
#include "../shared/structs/dataheader.h"


//template<typename S>
//HeaderDescription<S>::HeaderDescription(S&& header)
//    : _header(std::forward<DataHeader>(header))
//{}


template<typename S>
HeaderDescription<S>::HeaderDescription(DataHeader header)
  : _header(header)
  , _emptyMsgLen{sizeof(header.prefix) + // Корректный способ посчитать sizeof структуры
                 sizeof(header.type) +
                 sizeof(header.len) +
                 sizeof(header.postfix)} {
  // Подготовка последовательностей символов для контроля начала и
  // конца сообщения в формате std::string :
  auto pref = htons(header.prefix);
  //    _prefixStr.resize(2);
  auto sz = sizeof(header.prefix);
  _prefixStr.resize(sz);
  memcpy(&_prefixStr[0], &pref, sz);

  auto postf = htons(header.postfix);
  sz = sizeof(header.postfix);
  _postfixStr.resize(sz);
  memcpy(&_postfixStr[0], &postf, sz);

  qDebug() << "Sizes:" << _emptyMsgLen << "," << sizeof(header.prefix)
           << "and" << sizeof(header.postfix);
}


template<typename S>
HeaderDescription<S>::HeaderDescription(EmptyHeader header)
  : _header(header) {
}


template<typename S>
HeaderDescription<S>::~HeaderDescription() {
  qDebug() << "HeaderDescription<S>::~dtor() called";
}


template<>
std::size_t HeaderDescription<DataHeader>::getLen(const std::string& dataWithHeader,
                                                  std::size_t posFrom,
                                                  std::size_t size) const {
  std::string bytes = dataWithHeader.substr(posFrom, size);
  std::reverse(bytes.begin(), bytes.end());
  std::size_t len;
  memcpy(&len, &bytes[0], size);

  return len;
}


//template<>
//ulong HeaderDescription<EmptyHeader>::getLen(std::string &currentDataBlock) {
//  return 15;
//}


template<typename S>
std::size_t HeaderDescription<S>::prefixPos(const std::string& dataWithHeader) const noexcept {
  return dataWithHeader.find(_prefixStr);
}


template<typename S>
std::string HeaderDescription<S>::postfixStr() const noexcept {
  return _postfixStr;
}


template<typename S>
std::size_t HeaderDescription<S>::emptyMsgLen() const noexcept {
  return _emptyMsgLen;
}
