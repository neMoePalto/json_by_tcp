#pragma once

#include <string>


class AbstractP {
public:
  AbstractP(unsigned short num)
    : _num(num) {
  }

  virtual ~AbstractP() = default;

  virtual void initStruct(std::string& data, unsigned len) = 0;
  virtual void clearCollection() noexcept = 0;

  // Простой вариант обработки сериализованных объектов:
  //    virtual void useData() = 0;
  // Проблема в том, что виртуальная функция не может быть шаблонной:
  //    virtual std::vector<T> getData() = 0;

  unsigned short getNum() const noexcept {
    return _num;
  }

protected:
  unsigned short _num;
};
