#include "widget.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QTime>

#include "all_struct_parser/datahandler.h"
#include "parsersmanager.h"
#include "tcpserver.h"


Widget::Widget() {
  quint16 defaultPort = 9002; // TODO: Заменить на загрузку/сохранение параметра прошлой сессии
  quint32 defaultRestartValue = 0;
  _dataHandler = std::make_unique<DataHandler>();

  // --------------------- GUI --------------------:
  QFont arial_13B("Monospace", 13, QFont::DemiBold);
  QFont arial_12B("Monospace", 12, QFont::Bold);
  QFont arial_12("Arial",      12, QFont::Normal);
  QFont arial_14("Arial",      14, QFont::Normal);
  QFont arial_15("Arial",      15, QFont::Normal);
  QFont arial_16("Arial",      16, QFont::Normal);
  QFont arial_16B("Arial",     16, QFont::DemiBold);
  QFont arial_20B("Arial",     20, QFont::DemiBold);
  this->setFont(arial_12);

  QGridLayout* gridl = new QGridLayout(this);
  QLabel* lbTitle = new QLabel("Server");
  lbTitle->setFont(arial_20B);
  _lbCurrentPort = new QLabel();
  _lbCurrentPort->setFont(arial_16B);
  gridl->addWidget(lbTitle,          0, 0,   1, 1);
  gridl->addWidget(_lbCurrentPort,   0, 2,   1, 1);

  QString _step;
  QLabel* lbLeft1 = new QLabel(tr("Номер порта tcp-сервера:"));
  _step.setNum(defaultPort);
  _lePort = new QLineEdit();
  _lePort->setText(_step);
  _lePort->setFixedWidth(70);
  _pbConnectionStatus = new QPushButton();
  _pbConnectionStatus->setPalette(_red0);
  _pbConnectionStatus->setFixedWidth(40);
  _pbConnectionStatus->setCheckable(false);

  QHBoxLayout* hLay = new QHBoxLayout();
  hLay->addWidget(_lePort);
  hLay->addWidget(_pbConnectionStatus);
  gridl->addWidget(lbLeft1, 1, 0,   1, 1);
  gridl->addLayout(hLay,    1, 2,   1, 1, Qt::AlignLeft);

  QLabel *lbLeft2 = new QLabel(/*tr("Интервал авторестарта, от 4 сек \n(0 - авторестар отключен):")*/);
  _step.setNum(defaultRestartValue);
  gridl->addWidget(lbLeft2,         2, 0,   1, 1);
  _pbClearOutput = new QPushButton(tr("Очистить вывод"));
  _pbClearOutput->setFixedSize(150, 70);
  gridl->addWidget(_pbClearOutput, 3, 0,   2, 2);

  _pbStart = new QPushButton(tr("Запустить\nсервер"));
  _pbStart->setPalette(_green1);
  _pbStart->setFixedSize(250, 100);
  gridl->addWidget(_pbStart, 3, 2,   2, 2);

  QLabel *lb_33 = new QLabel(tr("Принятые блоки данных:"));
  _teStatistics = new QTextEdit();
  //    _teStatistics->setMinimumSize(500, 150);
  _teStatistics->setFixedWidth(500);

  _teStatistics->setPalette(_green1);
  _teStatistics->setFont(arial_15);
  QLabel *lbErrors = new QLabel(tr("Отладочная информация:"));
  _teErrors = new QTextEdit();
  //    _teErrors->setMinimumSize(400, 150);
  _teErrors->setFixedWidth(500);
  _teErrors->setPalette(_red1);
  _teErrors->setFont(arial_12B);
  gridl->addWidget(lb_33,          0, 4,   1, 2, Qt::AlignCenter);
  gridl->addWidget(_teStatistics,  1, 4,  14, 2);
  gridl->addWidget(lbErrors,      15, 4,   1, 2, Qt::AlignCenter);
  gridl->addWidget(_teErrors,     16, 4,   2, 2);

  // Описание таблицы:
  _tableStatistics = new QTableWidget(0, 4, this);
  _tableStatistics->setFont(arial_12B);
  QStringList headers{tr("Тип\nобъекта"),
                      tr("Количество\nобъектов"),
                      tr("Общее время\nразбора сообщения, с"),
                      tr("Среднее время сериализации\nодного объекта, с")};
  _tableStatistics->setHorizontalHeaderLabels(headers);
  _tableStatistics->setColumnWidth(0, 150);
  _tableStatistics->setColumnWidth(1, 150);
  _tableStatistics->setColumnWidth(2, 200);
  _tableStatistics->setColumnWidth(3, 260);
  _tableStatistics->setFixedWidth(780);
  auto header = _tableStatistics->horizontalHeader();
  header->setFixedHeight(46);
  gridl->addWidget(_tableStatistics,  5, 0,   13, 3);
  // ---------------------------------------
  setLayout(gridl);
  setFixedWidth(1315);
  setMinimumHeight(800);
  connect(_pbStart,       &QPushButton::clicked, this, &Widget::slotStartServer);
  connect(_pbClearOutput, &QPushButton::clicked, this, &Widget::clearOutput);

  _server = std::make_unique<TcpServer>(3);
  connect(_server.get(), &TcpServer::haveData,           this, &Widget::processMsg);
  connect(_server.get(), &TcpServer::listenPort,         this, &Widget::showServPort);
  connect(_server.get(), &TcpServer::clientConnected,    this, &Widget::slotCliConnected);
  connect(_server.get(), &TcpServer::clientDisconnected, this, &Widget::slotCliDisconnected);
  connect(_server.get(), &TcpServer::portIsBusy,         this, &Widget::slotPortIsBusy);

  slotStartServer();
}


Widget::~Widget() {
  // Члены-данные класса, обернутые в unique_ptr, будут иметь uncomplete type
  // без явного определения деструктора. Это плата за возможность использовать
  // unique_ptr<T> в хедере с предварительным объявлением T (т.е. без инклуда
  // хедера класса T).
}


void Widget::showEvent(QShowEvent* /*event*/) {
  // Т.к. метод shared_from_this() не может быть использован в конструкторе
  // класса Widget, конструировать объекты некоторых классов можно здесь.
}


void Widget::printJsonObjAmount(std::size_t amount) const noexcept {
  _teStatistics->setTextColor(QColor(160,60,0));
  _teStatistics->append(QTime::currentTime().toString("hh:mm:ss.zzz") +
                        tr("  Сериализовано ") + QString::number(amount) + tr(" объектов."));
  _teStatistics->setTextColor(Qt::black);
}


void Widget::printTimeAndSizeInfo(std::size_t msgSize) const noexcept {
  _teStatistics->append(QTime::currentTime().toString("hh:mm:ss.zzz") +
                        tr("  принято ") + QString::number(msgSize) + tr(" байт"));
}


void Widget::printParsingResults(MessageParsingResult info) {
  DataType type;
  uint amount;
  double wholeMsgTime;
  double objAverageTime;
  std::tie(type, amount, wholeMsgTime, objAverageTime) = info;

  printJsonObjAmount(amount);

  QString str;
  if (type == DataType::Json) {
    str = "JSON";
  } else if (type == DataType::Struct) {
    str = "Struct";
  }

  _tableStatistics->insertRow(_tableStatistics->rowCount());
  addDataItemToRow(0, str);
  addDataItemToRow(1, amount);
  addDataItemToRow(2, wholeMsgTime);
  addDataItemToRow(3, objAverageTime);
  //    auto item = new QTableWidgetItem();
  //    item->setData(Qt::DisplayRole, amount);
  //    item->setTextAlignment(Qt::AlignCenter);
  //    _tableStatistics->setItem(_tableStatistics->rowCount() - 1, 1, item);
}


void Widget::addDataItemToRow(int column, const QVariant& data) {
  auto item = new QTableWidgetItem();
  item->setData(Qt::DisplayRole, data);
  item->setTextAlignment(Qt::AlignCenter);
  _tableStatistics->setItem(_tableStatistics->rowCount() - 1, column, item);
}


void Widget::processMsg(std::vector<char>& data, TcpPort remoteCliPort) {
  printTimeAndSizeInfo(data.size());
  auto* parserManager = parserManagerForConnecton(remoteCliPort);
  parserManager->parseMsg(std::move(data));
  //    parser->printSturctsContent();
  _dataHandler->handle<DataOne>(parserManager->parsers());
  _dataHandler->handle<Struct001>(parserManager->parsers());
  _dataHandler->handle<DataTwo>(parserManager->parsers());
}


ParsersManager<Widget::Header, Widget::PFamily>* Widget::parserManagerForConnecton(TcpPort port) {
  const auto it = _parserManagers.find(port);
  if (it == _parserManagers.end()) {
    auto pm = ParsersManager<Header, PFamily>::create(shared_from_this(),
                                                      DataHeader{0x1002, 0xdddd, 0, 0x1003});
    _parserManagers.insert({port, pm});
    return pm.get();
  } else {
    return it->second.get();
  }
}


void Widget::slotStartServer() {
  QString st;
  st = _lePort->text();
  quint16 port = st.toUShort();
  if (port == 0) {
    qDebug() << "Widget::slotStartServer() error! Unkorrect port value!";
    return;
  }

  _server->restart(port);
  disconnect(_pbStart, &QPushButton::clicked, this, &Widget::slotStartServer);
  connect   (_pbStart, &QPushButton::clicked, this, &Widget::slotStopServer);

  st = tr("Tcp-сервер ожидает \nвходящие соединения");
  _pbStart->setText(st);
  _pbStart->setPalette(_yellow0);
}


void Widget::slotStopServer() {
  _server->close();
  disconnect(_pbStart, &QPushButton::clicked, this, &Widget::slotStopServer);
  connect   (_pbStart, &QPushButton::clicked, this, &Widget::slotStartServer);
  _pbStart->setText(tr("Активировать tcp-сервер"));
  _pbStart->setPalette(_green1);
  _pbConnectionStatus->setPalette(_red0);
}


void Widget::showServPort(quint16 port) {
  QString str;
  str.setNum(port);
  _lbCurrentPort->setText(str);
  clearLabels(ClearLabelsPolicy::ExceptFirst);
}


void Widget::slotCliConnected(quint16 port) {
  _teErrors->append("Client with port " + QString::number(port) + " connected");
  _pbConnectionStatus->setPalette(_green0);
}


void Widget::slotCliDisconnected(quint16 port) {
  _teErrors->append("Client with port " + QString::number(port) + " disconnected");
  _pbConnectionStatus->setPalette(_red0);
   // Удаляем связанный экземпляр парсера:
  _parserManagers.erase(port);
}


void Widget::slotPortIsBusy() {
  _server->close();
  emit quitFromApp();
}


void Widget::clearLabels(ClearLabelsPolicy fl) {
  if (fl == ClearLabelsPolicy::All) {
    _lbCurrentPort->clear();
  }

  _pbConnectionStatus->setPalette(_red0);
  _teStatistics->clear();
}


void Widget::clearOutput() {
  _teStatistics->clear();
  for (int i = _tableStatistics->rowCount(); i >= 0; --i) {
    _tableStatistics->removeRow(i);
  }
}
