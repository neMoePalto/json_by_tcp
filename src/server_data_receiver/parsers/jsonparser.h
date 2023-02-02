#pragma once

#include <string>
#include <vector>

#include <boost/property_tree/ptree.hpp>

#include "abstractparser.h"


class JsonParser : public AbstractParser {
public:
  // Синтаксис, позволяющий не объявлять и не определять "пустой" конструктор
  // в производном классе. Идеально для случая, когда задача такого ctor'a
  // сводится к транизиту параметоров в ctor базового класса:
  // using AbstractParser::AbstractParser;

  JsonParser(std::weak_ptr<ParsersManager<DataHeader, AbstractParser>> pm);
  ~JsonParser() override;

  void clearCollection() override;
  void createObject(std::string &data, std::size_t posEnd) override;
  void readBlocks(std::string &&data) override;

private:
  const std::string _delimiterJsonJson{ "}\n{" };
  std::string       _delimiterJsonPostfix{};
  std::vector<boost::property_tree::ptree> _jsonObjects{};
};

#include "jsonparser_impl.h"
