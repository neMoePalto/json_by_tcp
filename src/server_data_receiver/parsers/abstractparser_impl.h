#pragma once

//#ifndef ABSTRACTPARSER_H
//#error "Do not include this file directly! Use abstractparser.h!"
//#endif

#include "abstractparser.h"


AbstractParser::AbstractParser(std::weak_ptr<ParsersManager<DataHeader, AbstractParser>> pm)
  : _parserManager(pm) {
  _oneObjectSerializingTimer = std::make_shared<AverageTime>();
  _wholeMessageParsingTimer  = std::make_shared<AverageTime>();
}


void AbstractParser::setTotalLen(std::size_t len) noexcept {
  _totalLen = len;
}


void AbstractParser::fixStartTime() {
  _wholeMessageParsingTimer->fixStartTime();
}
