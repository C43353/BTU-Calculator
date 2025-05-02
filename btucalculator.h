#ifndef BTUCALCULATOR_H
#define BTUCALCULATOR_H

#include <QMainWindow>
#include <qlineedit.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class BTUCalculator;
}
QT_END_NAMESPACE

class BTUCalculator : public QMainWindow
{
    Q_OBJECT

public:
    BTUCalculator(QWidget *parent = nullptr);
    ~BTUCalculator();

private slots:
    double getLineEditValue(QLineEdit* lineEdit);
    void updatePlaceholders();
    void on_calculateButton_clicked();

private:
    Ui::BTUCalculator *ui;
    const double WattBTU = 3.412; // 1 watt = 3.412 BTU/hr
    const double AirDensity = 1.225; // kg/m^3
    const double AirSpecificHeat = 1.006; // J/kg.K
};
#endif // BTUCALCULATOR_H
