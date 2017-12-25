#include "onshitsudokei.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  Onshitsudokei w;
  w.show();

  return a.exec();
}
