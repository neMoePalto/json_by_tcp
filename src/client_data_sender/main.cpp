#include <QApplication>

#include "gui/widget.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  Widget w;
  w.show();

  return a.exec();
}
