#pragma once

#include <string>
#include <vector>

#include "abstractparser.h"


template<typename T>
class StructParser : public AbstractParser {
public:
  //    using AbstractParser<S>::AbstractParser;
  StructParser(std::weak_ptr<ParsersManager<DataHeader, AbstractParser>> pm);
  ~StructParser() override;

  void clearCollection() override;
  void createObject(std::string &data, std::size_t pos) override;
  void readBlocks(std::string &&data) override;

private:
  std::vector<T> _structObjects{};
};

#include "structparser_impl.h"
