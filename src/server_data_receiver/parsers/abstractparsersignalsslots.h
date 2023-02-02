#pragma once

#include <QObject>


enum class DataType {
  Json,
  Struct
};


using MessageParsingResult = std::tuple<DataType, ulong, double, double>;

class AbstractParserSignalsSlots : public QObject {
  Q_OBJECT

signals:
  void messageParsingComplete(MessageParsingResult);
  void parsingError(QString); // Unused now
};
