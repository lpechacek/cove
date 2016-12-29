#ifndef USING_PCH
#include <QTranslator>
#include <QApplication>
#include <QTextCodec>
#endif /* USING_PCH */

#include "mainform.H"

int main(int argc, char** argv)
{
  QApplication a(argc, argv);

  mainForm w;
  if(argc > 1)
  {
    w.loadImage(argv[1]);
  }

  w.show();

  a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
  return a.exec();
}
