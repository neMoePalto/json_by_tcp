#pragma once

#include <map>
#include <memory>

#include <QDebug>

#include "../shared/structs/dataheader.h"
#include "headerdescription.h"
#include "parsers/abstractparser.h"


/*
 * Для определенности введем терминологию:
 * - сообщение - единица информационного обмена, существующая на уровне приложения.
 * Для реализации возможности выделения (и последующего разбора) сообщения
 * в состав сообщения добавляются префикс, длина информационной части и постфикс.
 * - tcp-дейтаграмма - "кусок" последовательности байт, полученный из tcp-потока.
 *
 * При разборе содержимого отдельной tcp-дейтаграммы возникают несколько ситуаций:
 * 1) все сообщение лежит в одной дейтаграмме. Буферизация на стороне отправителя
 * не проводилась, поэтому никаких "кусков" следующих сообщений в данном сегменте
 * не содержится. Самый простой случай.
 * 2) в одной дейтаграмме лежит несколько сообщений. Т.е. буферизация проводилась,
 * но, по случайности, сообщения разделились ровно. Случай, возникающий
 * довольно редко, т.к. tcp-протокол ничего не знает о служебных полях сообщения,
 * которые мы сами и ввели.
 * 3) в одном сегменте лежит одно или несколько сообщений, после которых идет еще
 * "кусок" следующего сообщения. Случай будет регулярно возникать, если длина
 * сообщения очень мала (100-300 байт). Причина возникновения - tcp-протокол
 * иногда буфферизирует маленькие сообщения и отправляет их в одном сегменте.
 * 4) сообщение не поместилось в один сегмент (мы считываем только его часть).
 * Самый частый случай.
 */

class Widget;

template<typename S, typename PFamily>
class ParsersManager : public std::enable_shared_from_this<ParsersManager<S, PFamily>> {
private:
  explicit ParsersManager(std::weak_ptr<Widget> w, S header);
  void init();

public:
  static std::shared_ptr<ParsersManager> create(std::weak_ptr<Widget> w, S header);

  ParsersManager(const ParsersManager& other) = delete;
  ParsersManager& operator=(const ParsersManager& other) = delete;
  ~ParsersManager();

  void parseMsg(std::vector<char> &&data);
  void savePieceOfData(std::string&& piece);
  void readMsgFromBeginning(std::string &&data, S* ptr = nullptr);
  const HeaderDescription<S>* headerDescription() const noexcept;
  void printSturctsContent(); // Unused now
  const std::map<unsigned short, std::shared_ptr<PFamily>>* parsers() const noexcept;

private:
  PFamily* chooseParserByDataType(unsigned short type) const;
  std::string testData() {
    return {};
  }

private:
  // Два эквивалентных способа объявления указателей:
  // Важно понимать, что использование в качестве типа умного указателя
  // базового класса ЛУЧШЕ, чем использование в качестве типа
  // сложного шаблонного класса-потомка. Это связано с тем, что
  // сложный тип нуждается в двух дополнительных объявлениях классов.
  // После этих объявлений проект может просто не собраться.
  //    std::shared_ptr<StructParser<SomeStruct>> _structParser;
  //    ShPtrAbstractParser _structParser;
  std::map<unsigned short, std::shared_ptr<PFamily>> _dataParsers;
  PFamily* _currentParser;
  std::weak_ptr<Widget> _widget;
  std::shared_ptr<HeaderDescription<S>> _header;
  std::string _pieceOfData;
};


ushort convert(const std::string& type) {
  if (type.size() != sizeof(ushort)) {
    // TODO: Исключение было бы лучше (throw std::bad...)
    return 0;
  }

  ushort num;
  memcpy(&num, type.data(), sizeof(num));
  return htons(num);
}


template<>
void ParsersManager<DataHeader, AbstractParser>::readMsgFromBeginning(std::string &&data, DataHeader* /*ptr*/) {
  // Проверяем сообщение на наличие префикса:
  if (_header->prefixPos(data) == 0) {
    auto type = convert(data.substr(2, 2));
    _currentParser = chooseParserByDataType(type);
    if (_currentParser == nullptr) {
      qDebug() << QObject::tr("Ошибка в заголовке сообщения: невозможно определить тип данных");
      return;
    }

    _currentParser->fixStartTime();

    _currentParser->setTotalLen(_header->getLen(data, 4, 8));
    qDebug() << "Data len getting from header: " << _header->getLen(data, 4, 8);
    data.erase(0, 12);
    _currentParser->clearCollection();
  }

  if (_currentParser == nullptr) {   // В общем случае это условие не должно выполняться:
    qDebug() << QObject::tr("Активный парсер почему-то не выбран. Разбор сообщения прерван.");
    return;
  }

  _currentParser->readBlocks(std::move(data));
}

#include "parsersmanager_impl.h"
