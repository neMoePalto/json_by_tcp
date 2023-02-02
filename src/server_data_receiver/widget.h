#pragma once

#include <map>
#include <memory>

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QWidget>

#include "../shared/structs/dataheader.h"
#include "parsers/abstractparsersignalsslots.h"


enum class ClearLabelsPolicy {
  All,
  ExceptFirst
};


template<typename S, typename PFamily> class ParsersManager;
class AbstractParser;
class AbstractP;
class DataHandler;
class TcpServer;
class QTime;

class Widget : public QWidget,
               public std::enable_shared_from_this<Widget> {
  Q_OBJECT

private:
  using TcpPort = unsigned short;
  using Header  = DataHeader;
  using PFamily = AbstractParser;
//  using PFamily = AbstractP;
//  using Header = EmptyHeader;
//  using ptrFnProcessing = void(Widget::*)(QByteArray ba);

public:
  Widget();
  ~Widget();

  void processMsg(std::vector<char>& data, TcpPort remoteCliPort);
  void slotStartServer();
  void slotCliConnected(quint16 port);
  void slotCliDisconnected(quint16 port);
  void slotPortIsBusy();
  void showServPort(quint16 port);
  void printParsingResults(MessageParsingResult info);

signals:
  void quitFromApp();

private:
  void slotStopServer();
  void clearOutput();

  // TODO: Подумать над названием метода. Также: внутри метода используется
  // инф. о номере ряда, в который добавляются данные. Это нехорошо.
  void addDataItemToRow(int column, const QVariant &data);

  void clearLabels(ClearLabelsPolicy fl = ClearLabelsPolicy::All);
  void printJsonObjAmount(std::size_t amount) const noexcept;
  void printTimeAndSizeInfo(std::size_t msgSize) const noexcept;
  ParsersManager<Header, PFamily>* parserManagerForConnecton(TcpPort port);
  void showEvent(QShowEvent* /*event*/) override;

private:
  std::map<TcpPort, std::shared_ptr<ParsersManager<Header, PFamily>>> _parserManagers;
  std::unique_ptr<TcpServer>   _server;
  std::unique_ptr<DataHandler> _dataHandler;

  // ----------- GUI -----------:
  QTableWidget* _tableStatistics;
  QTextEdit*    _teStatistics;
  QTextEdit*    _teErrors;
  QLineEdit*    _lePort;
  QLabel*       _lbCurrentPort;
  QPushButton*  _pbStart;
  QPushButton*  _pbConnectionStatus;
  QPushButton*  _pbClearOutput;

  const QPalette _green0  = Qt::green;
  const QPalette _red0    = Qt::red;
  const QPalette _green1  = QPalette(QColor(110,140,60));
  const QPalette _red1    = QColor(200,110,70);
  const QPalette _gray0   = Qt::gray;
  const QPalette _yellow0 = Qt::yellow;
};
