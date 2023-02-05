#pragma once

#include <map>
#include <memory>
#include <string>

#include <boost/property_tree/ptree.hpp>

#include <QString>
#include <QWidget>

#include "../shared/structs/somestruct.h"


using namespace boost::property_tree;

class QLineEdit;
class QTextEdit;
class NetSettingsDialog;
class TcpSmartClient;

class Widget : public QWidget {
  Q_OBJECT

public:
  Widget();
  ~Widget();
  void slotClientRestart();

private:
  enum class DataType {
    Json,
    Struct
  };

  void sendStructData();
  void sendJsonData();
  ptree jsonFromFile(const std::string& fileName) const;
  void initHeaderParts() noexcept;
  void updateGui(qint64 dataSize, DataType type);

  template<typename T>
  std::string convertLen(T len) const;
  void addTestData(SomeStruct& mySturct) noexcept;

private:
  std::unique_ptr<TcpSmartClient>    _smartClient;
  NetSettingsDialog*                 _settingsWidget;
  QTextEdit*                         _teSendingResult;
  QLineEdit*                         _leObjAmount;
  const QString                      _defaultObjAmount = "1";

  using TypeInfo = std::tuple<std::string, QString>;
  const std::map<DataType, TypeInfo> _types;
  const unsigned short               _prefix  = 0x1002;
  const unsigned short               _postfix = 0x1003;
  std::string                        _prefixStr;
  std::string                        _postfixStr;
};
