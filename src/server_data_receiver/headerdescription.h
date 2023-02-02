#pragma once

#include <string>


struct DataHeader;
struct EmptyHeader;

template<typename S>
class HeaderDescription {
public:
  //    HeaderDescription(S&& header);
  HeaderDescription(DataHeader header);
  HeaderDescription(EmptyHeader header);
  ~HeaderDescription();

  std::size_t getLen(const std::string& dataWithHeader, std::size_t posFrom, std::size_t size) const;
  std::size_t prefixPos(const std::string& dataWithHeader) const noexcept;
  std::string postfixStr() const noexcept;
  std::size_t emptyMsgLen() const noexcept;

private:
  const S           _header;
  //    const std::size_t _emptyMsgLen = 14;
  const std::size_t _emptyMsgLen = 0;
  std::string       _prefixStr;
  std::string       _postfixStr;
};

//using HdrDescrDH = HeaderDescription<DataHeader>;
//using HdrDescrEmpH = HeaderDescription<EmptyHeader>;

#include "headerdescription_impl.h"
