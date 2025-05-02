#include "btucalculator.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BTUCalculator w;
    w.show();
    return a.exec();
}
