#pragma once

#include <memory>
#include <string>

#include <QString>

#include "abstractparsersignalsslots.h"
#include "averagetime.h"


template<typename S, typename PFamily> class ParsersManager;
struct DataHeader;

class AbstractParser : public AbstractParserSignalsSlots {
public:
  AbstractParser(std::weak_ptr<ParsersManager<DataHeader, AbstractParser>> pm);
  virtual ~AbstractParser() = default;

  virtual void clearCollection() = 0;
  virtual void createObject(std::string &data, std::size_t posEnd) = 0;
  virtual void readBlocks(std::string &&data) = 0;

  void setTotalLen(std::size_t len) noexcept;
  void fixStartTime();

protected:
  std::weak_ptr<ParsersManager<DataHeader, AbstractParser>> _parserManager;
  std::size_t _totalLen = 0;

  std::shared_ptr<AverageTime> _oneObjectSerializingTimer;
  std::shared_ptr<AverageTime> _wholeMessageParsingTimer;
};

#include "abstractparser_impl.h"
