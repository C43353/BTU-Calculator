#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qlineedit.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    double getLineEditValue(QLineEdit* lineEdit);
    QString getLineEditPrint(QLineEdit* lineEdit);
    void updatePlaceholders();
    void savePDF();

private:
    Ui::MainWindow *ui;
    const double WattBTU = 3.412; // 1 watt = 3.412 BTU/hr
    const double AirDensity = 1.225; // kg/m^3
    const double AirSpecificHeat = 1.006; // J/kg.K
};
#endif // MAINWINDOW_H
