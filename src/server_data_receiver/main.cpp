#include <memory>

#include <QApplication>

#include "widget.h"


int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  auto w = std::make_shared<Widget>();
  QObject::connect(w.get(), &Widget::quitFromApp, &QCoreApplication::quit);
  w->show();

  return a.exec();
}
