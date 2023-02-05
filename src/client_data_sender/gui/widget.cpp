#include "widget.h"

#include <algorithm>
#include <string>

#include <netinet/in.h>

#include <boost/property_tree/json_parser.hpp>

#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

#include "../shared/structs/somestruct.h"
#include "gui/netsettingsdialog.h"
#include "tcpsmartclient.h"


Widget::Widget()
  : _types{{DataType::Json,   TypeInfo{"JJ", tr("JSON-объекты")}},
           {DataType::Struct, TypeInfo{"SS", tr("структуры")}}} {
  initHeaderParts();

  QFont arial_12    ("Arial",     12, QFont::Normal);
  QFont arial_14    ("Arial",     14, QFont::Normal);
  QFont noto_sans_11("Noto Sans", 11, QFont::Normal);
  setFont(noto_sans_11);
  setFixedWidth(880);

  QLabel* lb01 = new QLabel(tr("Количество объектов\n(не более 6000 шт):"));
  lb01->setFixedWidth(180);
  _leObjAmount = new QLineEdit(this);
  _leObjAmount->setFixedWidth(90);
  _leObjAmount->setText(_defaultObjAmount);

  auto* pbSendJson = new QPushButton(tr("Отправить данные\nв формате JSON"), this);
  pbSendJson->setMinimumSize(100, 80);

  QLabel* lb02 = new QLabel(tr("Отчеты по передаче данных:"));
  _teSendingResult = new QTextEdit(this);
  _teSendingResult->setFixedWidth(580);
  _teSendingResult->setFont(arial_14);
  auto* pbSendStruct = new QPushButton(tr("Отправить данные\nв виде структур"), this);
  pbSendStruct->setMinimumSize(100, 80);

  auto* pbOpenSettingsWidget = new QPushButton(tr("Изменить\nсетевые настройки"));
  pbOpenSettingsWidget->setMinimumSize(100, 60);
  _settingsWidget = new NetSettingsDialog(this);

  auto* grid = new QGridLayout(this);
  grid->addWidget(lb01,                   0, 0);
  grid->addWidget(_leObjAmount,           0, 1,   1, 2);
  grid->addWidget(pbOpenSettingsWidget,   0, 4,   2, 2);
  grid->addWidget(pbSendJson,             2, 3,   2, 3, Qt::AlignTop);
  grid->addWidget(lb02,                   1, 0,   1, 3, Qt::AlignBottom | Qt::AlignHCenter);
  grid->addWidget(_teSendingResult,       2, 0,   8, 3);
  grid->addWidget(pbSendStruct,           4, 3,   2, 3, Qt::AlignTop);
  setLayout(grid);

  slotClientRestart();

  connect(pbSendJson, &QPushButton::clicked,   this, &Widget::sendJsonData);
  connect(pbSendStruct, &QPushButton::clicked, this, &Widget::sendStructData);

  connect(pbOpenSettingsWidget, &QPushButton::clicked, [this]() {
    _settingsWidget->show();
  });
}


Widget::~Widget() {
}


void Widget::slotClientRestart() {
  _smartClient = std::make_unique<TcpSmartClient>(QHostAddress{_settingsWidget->ip()},
                                                  _settingsWidget->port());
  connect(_smartClient.get(), &TcpSmartClient::connected, [this]() {
    qDebug() << "connected with port" << QString::number(_settingsWidget->port());
  });
  _settingsWidget->hide();
}


void Widget::sendStructData() {
  SomeStruct testStruct;
  addTestData(testStruct);

  std::string data;
  // Вместо шаблонной функции здесь можно использовать htonl():
  std::string structLen = convertLen(static_cast<int>(sizeof(testStruct)));
  std::size_t dataBlockLen = structLen.size() + sizeof(testStruct);
  unsigned int filesAmount = _leObjAmount->text().toUInt();
  data.resize(filesAmount * dataBlockLen);
  for (std::size_t i = 0; i < data.size(); i += dataBlockLen) {
    // Для двух объектов std::string можно использовать replace:
    memcpy(&data[i], &structLen[0], structLen.size());
    memcpy(&data[i + structLen.size()], &testStruct, sizeof(testStruct));
  }

  if (filesAmount > 0) {
    qDebug() << "\nРазмер одной структуры с блоком длины ="
             << sizeof(testStruct) + structLen.size();
  }
  data = _prefixStr + std::get<0>(_types.at(DataType::Struct)) +
         convertLen(static_cast<long>(data.size())) + data + _postfixStr;
  qDebug() << "Общая длина сообщения =" << data.size();
  auto res = _smartClient->sendToServer(data.c_str(), data.size());
  updateGui(res, DataType::Struct);
}


void Widget::sendJsonData() {
//  ptree tree = jsonFromFile("../json_files/network.json");
  ptree tree = jsonFromFile("../json_files/some_struct.json");
//  qDebug() << tree.get<ushort>("agentLocalPort");

  std::stringstream oss;
  std::size_t filesAmount = _leObjAmount->text().toULong();
  for (std::size_t i = 0; i < filesAmount; ++i) {
    boost::property_tree::json_parser::write_json(oss, tree);
  }

  std::string data = oss.str();
  if (filesAmount > 0) {
    qDebug() << "data.size() = " << data.size()
             << "\nРазмер json-файла = " << static_cast<double>(data.size() / filesAmount);
  }

  // Удаляем пробелы из массива данных (это увеличивает скорость разбора
  // сообщения на стороне сервера ~ на 18%):
//  _sendingTimer->fixStartTime();
//  boost::erase_all(data, " ");
//  _sendingTimer->fixEndTime();
//  auto delta = _sendingTimer->getAverage();
//  qDebug() << "Time of whitespace-removing = " << QString::number(delta);

//  data.erase(boost::remove_if(data, ::isspace), data.end() - 2);
//  boost::remove_erase_if(str, boost::is_any_of("123")); // is_any_of(" \n")
  // Формируем сообщение:
  data = _prefixStr + std::get<0>(_types.at(DataType::Json)) +
         convertLen(static_cast<long>(data.size()) ) + data + _postfixStr;
  qDebug() << "packss size() WITH HEADER = " << data.size();

  auto res = _smartClient->sendToServer(data.c_str(), data.size());
  updateGui(res, DataType::Json);
}


ptree Widget::jsonFromFile(const std::string &fileName) const {
  // Удаление "пробелов" из считанного файла перед вызовом функции
  // read_json(). Решение не имеет смысла, т.к. write_json() возвращает
  // удаленные ранее "пробелы".
  //    std::ifstream stream;
  //    stream.open(fileName);
  //    std::stringstream data;
  //    std::string s;
  //    while (getline(stream, s)) {
  //        std::stringstream line(s);
  //        while (line >> s)
  //            data << s;
  //    }
  //    qDebug() << "Data: " << data.str().size() << '\n' << data.str().c_str();

  ptree jsonTree;
  try { // Обрабатываем ситуацию "нечитаемый файл":
    boost::property_tree::read_json(fileName, jsonTree);
  } catch (const json_parser_error& e) {
    qDebug() << "Can't parse file " << e.filename().c_str()
             << "\nMay be, syntax error or another one.";
    return jsonTree; // Стоит ли возвращать пустое дерево?
  }

  return jsonTree;
}


void Widget::initHeaderParts() noexcept {
  // 2 байта, определяющие начало сообщения:
  auto pref = htons(_prefix);
  _prefixStr.resize(sizeof(pref));
  memcpy(&_prefixStr[0], &pref, sizeof(pref));
  // 2 байта, определяющие конец сообщения:
  auto postf = htons(_postfix);
  _postfixStr.resize(sizeof(postf));
  memcpy(&_postfixStr[0], &postf, sizeof(postf));
}


void Widget::updateGui(qint64 dataSize, DataType type) {
  using ValueAndMeasureUnit = std::pair<long double, QString>;
  ValueAndMeasureUnit value;

  auto size = static_cast<long double>(dataSize);
  value = size > 1024 ? ValueAndMeasureUnit{size / 1024, tr(" Кбайт")}
                      : ValueAndMeasureUnit{size       , tr(" Байт")};
  if (value.first > 1024) {
    value = {value.first / 1024, tr(" Мбайт")};
  }

  QString amount = _leObjAmount->text().toInt() > 0 ? _leObjAmount->text() + tr(" шт.")
                                                    : QString::number(0);
  _teSendingResult->append("Формат данных: " +
                           std::get<1>(_types.at(type)) + ", " + amount + ",  " +
                           QString::number(static_cast<double>(value.first)) + value.second);
}


// TODO: Нужен ли здесь шаблон?
// Дописать шаблон так, чтобы он работал только с простыми типами:
template<typename T>
std::string Widget::convertLen(T len) const {
  std::string dataLen;
  dataLen.resize(sizeof(T));
  memcpy(&dataLen[0], &len, sizeof(T));
  std::reverse(dataLen.begin(), dataLen.end());
  //    qDebug() << len;

  return dataLen;
}


void Widget::addTestData(SomeStruct &mySturct) noexcept {
  mySturct.a = 0xCC;
  mySturct.b = 0xDD;
  mySturct.c = 0xAA;
  mySturct.d = 0xBB;
  mySturct.e = 0x0BB8;
  mySturct.f = 0xEE;

  mySturct.g = 0x30;
  mySturct.h = 0x01;
  mySturct.i = 0x31;
  mySturct.j = 0x32;
  mySturct.k = 0x04;
}
