#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QDoubleValidator>
#include <QFileDialog>
#include <QStandardPaths>
#include <QPrinter>
#include <QPainter>
#include <QImage>
#include <QPdfWriter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    // Setup float (numeric) only QLineEdit and default 0 values
    QDoubleValidator *doubleValidator = new QDoubleValidator(-1e100, 1e100, 100, this);
    doubleValidator->setNotation(QDoubleValidator::StandardNotation);

    QList<QLineEdit *> numericEdits = {
        ui->LELengthHC, ui->LEWidthHC, ui->LEHeightHC,
        ui->LENWindowHC, ui->LEEWindowHC, ui->LESWindowHC, ui->LEWWindowHC,
        ui->LEOccupantsHC, ui->LEEquipmentHC, ui->LELightHC,
        ui->LECoolAdjustHC, ui->LECoolCapacityHC,
        ui->LEWallAreaHC, ui->LEWindowAreaHC, ui->LECeilingAreaHC, ui->LEFloorAreaHC,
        ui->LETargetTempHC, ui->LEExternalTempHC,
        ui->LEVentilationHC, ui->LELeakageHC,
        ui->LEHeatAdjustHC, ui->LEHeatCapacityHC,
        ui->LEWallMaterialHC, ui->LEWindowMaterialHC,
        ui->LECeilingMaterialHC, ui->LEFloorMaterialHC
    };

    for (auto *edit : numericEdits) {
        edit->setValidator(doubleValidator);
        edit->setPlaceholderText(QString::number(0, 'f', 2));
    }


    // Setup Specific Placeholders
    ui->LETargetTempHC->setPlaceholderText(QString::number(19, 'f', 2));
    ui->LEExternalTempHC->setPlaceholderText(QString::number(5, 'f', 2));
    ui->LEVentilationHC->setPlaceholderText(QString::number(4, 'f', 2));
    ui->LELeakageHC->setPlaceholderText(QString::number(0.5, 'f', 2));
    ui->LECoolAdjustHC->setPlaceholderText(QString::number(10, 'f', 2));
    ui->LEHeatAdjustHC->setPlaceholderText(QString::number(10, 'f', 2));
    ui->LECoolCapacityHC->setPlaceholderText(QString::number(2500, 'f', 2));
    ui->LEHeatCapacityHC->setPlaceholderText(QString::number(1500, 'f', 2));


    // Setup Dropdown Boxes
    ui->LightTypeHC->addItem("Incandescent", 4.25);
    ui->LightTypeHC->addItem("Florescent", 2.8);
    ui->LightTypeHC->addItem("LED", 2);

    ui->WallMaterialHC->addItem("Solid Brick Wall", 2);
    ui->WallMaterialHC->addItem("Cavity Wall (No Insulation)", 1.5);
    ui->WallMaterialHC->addItem("Cavity Wall (Insulated)", 0.55);
    ui->WallMaterialHC->addItem("Modern Insulated Wall", 0.3);
    ui->WallMaterialHC->addItem("Timber Frame Wall (with Insulation)", 0.3);

    ui->WindowMaterialHC->addItem("Single Glazed", 5.8);
    ui->WindowMaterialHC->addItem("Double Glazed (old, air filled)", 3);
    ui->WindowMaterialHC->addItem("Double Glazed (modern, low-e)", 1.6);
    ui->WindowMaterialHC->addItem("Triple Glazed", 1);

    ui->CeilingMaterialHC->addItem("Uninsulated Loft", 3);
    ui->CeilingMaterialHC->addItem("Insulated Ceiling", 0.4);
    ui->CeilingMaterialHC->addItem("Modern Insulated Roof", 0.2);
    ui->CeilingMaterialHC->addItem("Warm Flata Roof (Insulated)", 0.25);

    ui->FloorMaterialHC->addItem("Uninsulated Solid Floor", 1);
    ui->FloorMaterialHC->addItem("Suspended Timber Floor", 1);
    ui->FloorMaterialHC->addItem("Insulated Ground-Bearing Slab", 0.25);
    ui->FloorMaterialHC->addItem("Retrofit Insulated Timber Floor", 0.3);

    ui->CoolUnitsHC->addItems({"Watts", "BTU"});
    ui->HeatUnitsHC->addItems({"Watts", "BTU"});


    // Setup Auto Calculations
    auto connectChanged = [this](QObject *obj) {
        connect(obj, SIGNAL(textChanged(QString)), this, SLOT(updatePlaceholders()));
    };

    for (auto *edit : numericEdits)
        connectChanged(edit);

    QList<QComboBox *> comboBoxes = {
        ui->LightTypeHC, ui->WallMaterialHC, ui->WindowMaterialHC,
        ui->CeilingMaterialHC, ui->FloorMaterialHC,
        ui->CoolUnitsHC, ui->HeatUnitsHC
    };

    for (auto *combo : comboBoxes)
        connect(combo, SIGNAL(currentTextChanged(QString)), this, SLOT(updatePlaceholders()));

    QList<QCheckBox *> checkboxes = {
        ui->NorthShadeHC, ui->EastShadeHC,
        ui->SouthShadeHC, ui->WestShadeHC
    };

    for (auto *box : checkboxes)
        connect(box, SIGNAL(stateChanged(int)), this, SLOT(updatePlaceholders()));

    connect(ui->pushButton, &QPushButton::pressed, this, &MainWindow::savePDF);
}


MainWindow::~MainWindow()
{
    delete ui;
}


// Function to get line edit values, gets placeholer values if empty
double MainWindow::getLineEditValue(QLineEdit* lineEdit) {
    return lineEdit->text().isEmpty() ?
               lineEdit->placeholderText().toDouble() :
               lineEdit->text().toDouble();
}

QString MainWindow::getLineEditPrint(QLineEdit* lineEdit) {
    return lineEdit->text().isEmpty() ?
               lineEdit->placeholderText() :
               lineEdit->text();
}

// Updates placeholders as text is input (use theory to make auto update (no need for calculate button))
void MainWindow::updatePlaceholders()
{
    double length = getLineEditValue(ui->LELengthHC);
    double width  = getLineEditValue(ui->LEWidthHC);
    double height = getLineEditValue(ui->LEHeightHC);

    double northArea = getLineEditValue(ui->LENWindowHC);
    double eastArea = getLineEditValue(ui->LEEWindowHC);
    double southArea = getLineEditValue(ui->LESWindowHC);
    double westArea = getLineEditValue(ui->LEWWindowHC);

    double northShade = ui->NorthShadeHC->isChecked() ? 1.0 : 1.4;
    double eastShade = ui->EastShadeHC->isChecked() ? 1.0 : 1.4;
    double southShade = ui->SouthShadeHC->isChecked() ? 1.0 : 1.4;
    double westShade = ui->WestShadeHC->isChecked() ? 1.0 : 1.4;

    double occupants = getLineEditValue(ui->LEOccupantsHC);
    double equipmentWatt = getLineEditValue(ui->LEEquipmentHC);
    double lightingWatts = getLineEditValue(ui->LELightHC);
    double coolAdjust = getLineEditValue(ui->LECoolAdjustHC);
    double coolCapacity = getLineEditValue(ui->LECoolCapacityHC);
    double lightingMult = ui->LightTypeHC->currentData().toDouble();

    double wallMultGuess = ui->WallMaterialHC->currentData().toDouble();
    ui->LEWallMaterialHC->setPlaceholderText(QString::number(wallMultGuess, 'f', 2));
    double windowMultGuess = ui->WindowMaterialHC->currentData().toDouble();
    ui->LEWindowMaterialHC->setPlaceholderText(QString::number(windowMultGuess, 'f', 2));
    double ceilingMultGuess = ui->CeilingMaterialHC->currentData().toDouble();
    ui->LECeilingMaterialHC->setPlaceholderText(QString::number(ceilingMultGuess, 'f', 2));
    double floorMultGuess = ui->FloorMaterialHC->currentData().toDouble();
    ui->LEFloorMaterialHC->setPlaceholderText(QString::number(floorMultGuess, 'f', 2));

    double wallMult = getLineEditValue(ui->LEWallMaterialHC);
    double windowMult = getLineEditValue(ui->LEWindowMaterialHC);
    double ceilingMult = getLineEditValue(ui->LECeilingMaterialHC);
    double floorMult = getLineEditValue(ui->LEFloorMaterialHC);

    double targetTemp = getLineEditValue(ui->LETargetTempHC);
    double externalTemp = getLineEditValue(ui->LEExternalTempHC);
    double ventRate = getLineEditValue(ui->LEVentilationHC);
    double leakRate = getLineEditValue(ui->LELeakageHC);
    double heatAdjust = getLineEditValue(ui->LEHeatAdjustHC);
    double heatCapacity = getLineEditValue(ui->LEHeatCapacityHC);

    double ceilingAreaGuess = length * width;
    double floorAreaGuess = ceilingAreaGuess;
    ui->LECeilingAreaHC->setPlaceholderText(QString::number(ceilingAreaGuess, 'f', 2));
    ui->LEFloorAreaHC->setPlaceholderText(QString::number(floorAreaGuess, 'f', 2));

    double windowAreaGuess = northArea + eastArea + southArea + westArea;
    ui->LEWindowAreaHC->setPlaceholderText(QString::number(windowAreaGuess, 'f', 2));

    double wallAreaGuess = (2 * length * height) + (2 * width * height) - windowAreaGuess;
    ui->LEWallAreaHC->setPlaceholderText(QString::number(wallAreaGuess, 'f', 2));

    double wallArea = getLineEditValue(ui->LEWallAreaHC);
    double windowArea = getLineEditValue(ui->LEWindowAreaHC);
    double ceilingArea = getLineEditValue(ui->LECeilingAreaHC);
    double floorArea = getLineEditValue(ui->LEFloorAreaHC);


    // Calculate Cooling BTU
    double roomArea = length * width * 3.2808399 * 3.2808399; // metres to feet
    double roomBTU = roomArea * 31.25;
    double roomWatt = roomBTU / WattBTU;

    double northWindowBTU = northArea * 164 * northShade;
    double northWindowWatt = northWindowBTU / WattBTU;
    double eastWindowBTU = eastArea * 625 * eastShade;
    double eastWindowWatt = eastWindowBTU / WattBTU;
    double southWindowBTU = southArea * 868 * southShade;
    double southWindowWatt = southWindowBTU / WattBTU;
    double westWindowBTU = westArea * 625 * westShade;
    double westWindowWatt = westWindowBTU / WattBTU;
    double windowCoolBTU = northWindowBTU + eastWindowBTU + southWindowBTU + westWindowBTU;
    double windowCoolWatt = northWindowWatt + eastWindowWatt + southWindowWatt + westWindowWatt;

    double occupantBTU = occupants * 600;
    double occupantWatt = occupantBTU / WattBTU;

    double equipmentBTU = equipmentWatt * WattBTU;

    double lightingBTU = lightingWatts * lightingMult;
    double lightingWatt = lightingBTU / WattBTU;

    double totalCoolingBTU = roomBTU + windowCoolBTU + occupantBTU + equipmentBTU + lightingBTU;
    double totalCoolingWatt = totalCoolingBTU / WattBTU;
    double peakCoolingBTU = totalCoolingBTU * (1 + coolAdjust / 100);
    double peakCoolingWatt = peakCoolingBTU / WattBTU;

    int numberCoolingUnits = 0;
    if (ui->CoolUnitsHC->currentText() == "BTU")
    {
        numberCoolingUnits = ceil(peakCoolingBTU / coolCapacity);
    }
    if (ui->CoolUnitsHC->currentText() == "Watts")
    {
        numberCoolingUnits = ceil(peakCoolingWatt / coolCapacity);
    }


    // Calculate Heating BTU
    double roomVolume = length * width * height;

    double diffTemp = abs(targetTemp - externalTemp);

    double wallWatt = wallArea * diffTemp * wallMult;
    double wallBTU = wallWatt * WattBTU;
    double windowHeatWatt = windowArea * diffTemp * windowMult;
    double windowHeatBTU = windowHeatWatt * WattBTU;
    double ceilingWatt = ceilingArea * diffTemp * ceilingMult;
    double ceilingBTU = ceilingWatt * WattBTU;
    double floorWatt = floorArea * diffTemp * floorMult;
    double floorBTU = floorWatt * WattBTU;
    double transmissionWatt = wallWatt + windowHeatWatt + ceilingWatt + floorWatt;
    double transmissionBTU = transmissionWatt * WattBTU;

    double ventWatt = AirDensity * AirSpecificHeat * (ventRate / (60*60)) * roomVolume * diffTemp;
    double ventBTU = ventWatt * WattBTU;
    double leakWatt = AirDensity * AirSpecificHeat * (leakRate / (60*60)) * roomVolume * diffTemp;
    double leakBTU = leakWatt * WattBTU;

    double totalHeatingWatt = transmissionWatt + ventWatt + leakWatt;
    double totalHeatingBTU = totalHeatingWatt * WattBTU;
    double peakHeatingWatt = totalHeatingWatt * (1 + heatAdjust / 100);
    double peakHeatingBTU = peakHeatingWatt * WattBTU;

    int numberHeatingUnits = 0;
    if (ui->CoolUnitsHC->currentText() == "BTU")
    {
        numberHeatingUnits = ceil(peakHeatingBTU / heatCapacity);
    }
    if (ui->CoolUnitsHC->currentText() == "Watts")
    {
        numberHeatingUnits = ceil(peakHeatingWatt / coolCapacity);
    }


    // Output results to text boxes
    ui->OutRoomHeatHC->setText(QString::number(roomWatt, 'f', 2));
    ui->OutWindowHeatHC->setText(QString::number(windowCoolWatt, 'f', 2));
    ui->OutOccupantHeatHC->setText(QString::number(occupantWatt, 'f', 2));
    ui->OutEquipmentHeatHC->setText(QString::number(equipmentWatt, 'f', 2));
    ui->OutLightHeatHC->setText(QString::number(lightingWatt, 'f', 2));
    ui->OutTotalCoolHC->setText(QString::number(totalCoolingWatt, 'f', 2));
    ui->OutPeakCoolHC->setText(QString::number(peakCoolingWatt, 'f', 2));
    ui->OutCoolUnitsHC->setText(QString::number(numberCoolingUnits, 'f', 2));

    ui->OutWallLossHC->setText(QString::number(wallWatt, 'f', 2));
    ui->OutWindowLossHC->setText(QString::number(windowHeatWatt, 'f', 2));
    ui->OutCeilingLossHC->setText(QString::number(ceilingWatt, 'f', 2));
    ui->OutFloorLossHC->setText(QString::number(floorWatt, 'f', 2));
    ui->OutTransmissionLossHC->setText(QString::number(transmissionWatt, 'f', 2));
    ui->OutVentilationLossHC->setText(QString::number(ventWatt, 'f', 2));
    ui->OutLeakageLossHC->setText(QString::number(leakWatt, 'f', 2));
    ui->OutTotalHeatHC->setText(QString::number(totalHeatingWatt, 'f', 2));
    ui->OutPeakHeatHC->setText(QString::number(peakHeatingWatt, 'f', 2));
    ui->OutHeatUnitsHC->setText(QString::number(numberHeatingUnits, 'f', 2));
}


void MainWindow::savePDF()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save PDF"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/output.pdf",
        tr("PDF Files (*.pdf)")
        );

    if (fileName.isEmpty())
        return;

    if (!fileName.endsWith(".pdf", Qt::CaseInsensitive))
        fileName += ".pdf";

    QPdfWriter writer(fileName);
    writer.setPageSize(QPageSize::A4);
    writer.setResolution(300);
    writer.setCreator("KAM Engineering Ltd.");
    QPainter painter(&writer);

    // Page dimensions and margins
    const int pageWidth = writer.width();
    const int pageHeight = writer.height();
    const int margin = 200;
    const int columnGap = 60;
    const int columnWidth = (pageWidth - 2 * margin - columnGap) / 2;
    const int lineSpacing = 60;
    const int sectionSpacing = 160;

    // Fonts
    QFont titleFont("Arial", 14, QFont::Bold);
    QFont headerFont("Arial", 11, QFont::Bold);
    QFont contentFont("Arial", 10);
    QFont footerFont("Arial", 8);

    QPen pen(QColor(102, 46, 145));
    painter.setPen(pen);

    // Draw background logo and corner logo
    QImage backgroundImage(":/Kam logo.jpg");
    if (!backgroundImage.isNull()) {
        QTransform rotation;
        rotation.rotate(60);
        QImage rotatedImage = backgroundImage.transformed(rotation);
        QImage scaledImage = rotatedImage.scaled(pageWidth, pageHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        painter.setOpacity(0.2);
        painter.drawImage(QPoint(0, 0), scaledImage);
        painter.setOpacity(1.0);


        QImage scaledLogo = backgroundImage.scaledToHeight(pageHeight/20, Qt::SmoothTransformation);
        painter.drawImage(QPoint(pageWidth-scaledLogo.width(), 0), scaledLogo);
    }

    // Draw F-Gas logo
    QImage FGasImage(":/REFCOM Certified logo.png");
    if (!FGasImage.isNull()) {
        QImage scaledFGas = FGasImage.scaledToHeight(pageHeight/20, Qt::SmoothTransformation);
        painter.drawImage(QPoint(0, 0), scaledFGas);
    }


    // --- Title ---
    painter.setFont(titleFont);
    painter.drawText(QRect(margin, margin / 2, pageWidth - 2 * margin, margin),
                     Qt::AlignHCenter,
                     "HVAC Load Calculation Report");

    // --- Branding ---
    int yTop = margin;
    painter.setFont(headerFont);
    painter.drawText(QRect(0, yTop, pageWidth, margin),
                     Qt::AlignHCenter | Qt::AlignTop,
                     "www.kam-surveys.co.uk | hello@kam-surveys.co.uk");

    // --- Initialize Column Coordinates ---
    int yLeft = yTop + sectionSpacing;
    int yRight = yTop + sectionSpacing;
    int yMain = yTop + 1.5 * sectionSpacing;
    int xLeft = margin;
    int xRight = margin + columnWidth + columnGap;

    // Lambda for drawing label-value pair
    auto writeLine = [&](int x, int &y, const QString &label, const QString &value, const QString &string3="", const QString &string4="", bool newLine=true) {
        painter.setFont(contentFont);
        painter.drawText(x, y, QString("%1: %2%3%4").arg(label, value, string3, string4));
        if (newLine) {
            y += lineSpacing;
        }
    };

    painter.setFont(headerFont);
    painter.drawText(xLeft, yMain, "Room Dimensions");
    painter.drawText(xRight, yMain, "Surface Information");
    yMain += lineSpacing;

    painter.setFont(contentFont);
    painter.drawText(xLeft, yMain, QString("%1: %2 m").arg("Length", getLineEditPrint(ui->LELengthHC)));
    painter.drawText(xRight, yMain, QString("%1: %2 m\u00B2").arg("Wall Surface Area", getLineEditPrint(ui->LEWallAreaHC)));
    yMain += lineSpacing;

    painter.setFont(footerFont);
    painter.drawText(xRight, yMain, QString("   %1 - %2: %3 W/m\u00B2K").arg(ui->WallMaterialHC->currentText(), "U-Value", getLineEditPrint(ui->LEWallMaterialHC)));
    painter.setFont(contentFont);
    painter.drawText(xLeft, yMain, QString("%1: %2 m").arg("Width", getLineEditPrint(ui->LEWidthHC)));
    yMain += lineSpacing;

    painter.drawText(xLeft, yMain, QString("%1: %2 m").arg("Height", getLineEditPrint(ui->LEHeightHC)));
    painter.drawText(xRight, yMain, QString("%1: %2 m\u00B2").arg("Window Surface Area", getLineEditPrint(ui->LEWindowAreaHC)));
    yMain += lineSpacing;

    painter.setFont(footerFont);
    painter.drawText(xRight, yMain, QString("   %1 - %2: %3 W/m\u00B2K").arg(ui->WindowMaterialHC->currentText(), "U-Value", getLineEditPrint(ui->LEWindowMaterialHC)));
    yMain += lineSpacing;

    painter.setFont(headerFont);
    painter.drawText(xLeft, yMain, "Window Areas & Shading");
    painter.setFont(contentFont);
    painter.drawText(xRight, yMain, QString("%1: %2 m\u00B2").arg("Ceiling Surface Area", getLineEditPrint(ui->LECeilingAreaHC)));
    yMain += lineSpacing;

    painter.drawText(xLeft, yMain, QString("%1: %2 m\u00B2 - %3").arg("North Window Area", getLineEditPrint(ui->LENWindowHC), ui->NorthShadeHC->isChecked() ? "Shaded" : "Unshaded"));
    painter.setFont(footerFont);
    painter.drawText(xRight, yMain, QString("   %1 - %2: %3 W/m\u00B2K").arg(ui->CeilingMaterialHC->currentText(), "U-Value", getLineEditPrint(ui->LECeilingMaterialHC)));
    yMain += lineSpacing;

    painter.setFont(contentFont);
    painter.drawText(xLeft, yMain, QString("%1: %2 m\u00B2 - %3").arg("East Window Area", getLineEditPrint(ui->LEEWindowHC), ui->EastShadeHC->isChecked() ? "Shaded" : "Unshaded"));
    painter.drawText(xRight, yMain, QString("%1: %2 m\u00B2").arg("Floor Surface Area", getLineEditPrint(ui->LEFloorAreaHC)));
    yMain += lineSpacing;

    painter.drawText(xLeft, yMain, QString("%1: %2 m\u00B2 - %3").arg("South Window Area", getLineEditPrint(ui->LESWindowHC), ui->SouthShadeHC->isChecked() ? "Shaded" : "Unshaded"));
    painter.setFont(footerFont);
    painter.drawText(xRight, yMain, QString("   %1 - %2: %3 W/m\u00B2K").arg(ui->FloorMaterialHC->currentText(), "U-Value", getLineEditPrint(ui->LEFloorMaterialHC)));
    yMain += lineSpacing;

    painter.setFont(contentFont);
    painter.drawText(xLeft, yMain, QString("%1: %2 m\u00B2 - %3").arg("West Window Area", getLineEditPrint(ui->LEWWindowHC), ui->WestShadeHC->isChecked() ? "Shaded" : "Unshaded"));
    yMain += lineSpacing;

    painter.setFont(headerFont);
    painter.drawText(xRight, yMain, QString("Temperature Conditions"));
    yMain += lineSpacing;

    painter.drawText(xLeft, yMain, QString("Internal Heat Loads"));
    painter.setFont(contentFont);
    painter.drawText(xRight, yMain, QString("%1: %2 \u00B0C").arg("Target Room Temperature", getLineEditPrint(ui->LETargetTempHC)));
    yMain += lineSpacing;

    painter.drawText(xLeft, yMain, QString("%1: %2").arg("No. of Occupants", getLineEditPrint(ui->LEOccupantsHC)));
    painter.drawText(xRight, yMain, QString("%1: %2 \u00B0C").arg("External Temperature", getLineEditPrint(ui->LEExternalTempHC)));
    yMain += lineSpacing;

    painter.drawText(xLeft, yMain, QString("%1: %2 W").arg("Equipment Load", getLineEditPrint(ui->LEEquipmentHC)));
    yMain += lineSpacing;

    painter.drawText(xLeft, yMain, QString("%1: %2 W - %3").arg("Lighting Load", getLineEditPrint(ui->LELightHC), ui->LightTypeHC->currentText()));
    painter.setFont(headerFont);
    painter.drawText(xRight, yMain, QString("Air Flow Information"));
    yMain += lineSpacing;

    painter.setFont(contentFont);
    painter.drawText(xRight, yMain, QString("%1: %2").arg("Ventilation (Desired) Air Replacements per Hour", getLineEditPrint(ui->LEVentilationHC)));
    yMain += lineSpacing;

    painter.drawText(xRight, yMain, QString("%1: %2").arg("Leakage (Undesired) Air Replacements per Hour", getLineEditPrint(ui->LELeakageHC)));
    yMain += lineSpacing;

    yMain += lineSpacing;

    painter.setFont(headerFont);
    painter.drawText(xLeft, yMain, QString("Cooling Unit Settings"));
    painter.drawText(xRight, yMain, QString("Heating Unit Settings"));
    yMain += lineSpacing;

    painter.setFont(contentFont);
    painter.drawText(xLeft, yMain, QString("%1: %2 %").arg("Peak Cooling Adjustment", getLineEditPrint(ui->LECoolAdjustHC)));
    painter.drawText(xRight, yMain, QString("%1: %2 %").arg("Peak Heating Adjustment", getLineEditPrint(ui->LEHeatAdjustHC)));
    yMain += lineSpacing;

    painter.drawText(xLeft, yMain, QString("%1: %2 W").arg("Cooling Capacity per Unit", getLineEditPrint(ui->LECoolCapacityHC)));
    painter.drawText(xRight, yMain, QString("%1: %2 W").arg("Heating Capacity per Unit", getLineEditPrint(ui->LEHeatCapacityHC)));


    yLeft = yMain;
    yRight = yMain;

    yLeft += sectionSpacing;

    // --- Cooling Summary (Left Column) ---
    painter.setFont(headerFont);
    painter.drawText(xLeft, yLeft, "Cooling Summary");
    yLeft += lineSpacing;

    writeLine(xLeft, yLeft, "Room Heat Load (W)", ui->OutRoomHeatHC->toPlainText());
    writeLine(xLeft, yLeft, "Window Heat Gain (W)", ui->OutWindowHeatHC->toPlainText());
    writeLine(xLeft, yLeft, "Occupant Heat Gain (W)", ui->OutOccupantHeatHC->toPlainText());
    writeLine(xLeft, yLeft, "Equipment Load (W)", ui->OutEquipmentHeatHC->toPlainText());
    writeLine(xLeft, yLeft, "Lighting Load (W)", ui->OutLightHeatHC->toPlainText());
    writeLine(xLeft, yLeft, "Total Cooling Load (W)", ui->OutTotalCoolHC->toPlainText());
    writeLine(xLeft, yLeft, "Peak Cooling Load (W)", ui->OutPeakCoolHC->toPlainText());
    writeLine(xLeft, yLeft, "Cooling Units Required", ui->OutCoolUnitsHC->toPlainText());

    yRight += sectionSpacing;

    // --- Heating Summary (Right Column) ---
    painter.setFont(headerFont);
    painter.drawText(xRight, yRight, "Heating Summary");
    yRight += lineSpacing;

    writeLine(xRight, yRight, "Wall Heat Loss (W)", ui->OutWallLossHC->toPlainText());
    writeLine(xRight, yRight, "Window Heat Loss (W)", ui->OutWindowLossHC->toPlainText());
    writeLine(xRight, yRight, "Ceiling Heat Loss (W)", ui->OutCeilingLossHC->toPlainText());
    writeLine(xRight, yRight, "Floor Heat Loss (W)", ui->OutFloorLossHC->toPlainText());
    writeLine(xRight, yRight, "Transmission Loss (W)", ui->OutTransmissionLossHC->toPlainText());
    writeLine(xRight, yRight, "Ventilation Loss (W)", ui->OutVentilationLossHC->toPlainText());
    writeLine(xRight, yRight, "Leakage Loss (W)", ui->OutLeakageLossHC->toPlainText());
    writeLine(xRight, yRight, "Total Heating Load (W)", ui->OutTotalHeatHC->toPlainText());
    writeLine(xRight, yRight, "Peak Heating Load (W)", ui->OutPeakHeatHC->toPlainText());
    writeLine(xRight, yRight, "Heating Units Required", ui->OutHeatUnitsHC->toPlainText());

    // --- Footer ---
    painter.setFont(footerFont);
    QString footer1 = "This BTU calculated based on the information gathered during the onsite survey of your property and is intended as a guide to help determine the";
    QString footer2 = "appropriate air conditioning capacity for the space in question. Accurate sizing ensures optimal comfort, energy efficiency, and performance.";
    QString footer3 = "For more tailored recommendations or to discuss your requirements further, please contact our team.";
    QString footer5 = "KAM Engineering Ltd | Generated on: " + QDate::currentDate().toString("dd MMMM yyyy");

    QRect footer1Rect(0, pageHeight - margin / 2 - 4*lineSpacing, pageWidth, lineSpacing);
    painter.drawText(footer1Rect, Qt::AlignHCenter | Qt::AlignVCenter, footer1);

    QRect footer2Rect(0, pageHeight - margin / 2 - 3*lineSpacing, pageWidth, lineSpacing);
    painter.drawText(footer2Rect, Qt::AlignHCenter | Qt::AlignVCenter, footer2);

    QRect footer3Rect(0, pageHeight - margin / 2 - 2*lineSpacing, pageWidth, lineSpacing);
    painter.drawText(footer3Rect, Qt::AlignHCenter | Qt::AlignVCenter, footer3);

    QRect footer5Rect(0, pageHeight - margin / 2 - 20, pageWidth, lineSpacing);
    painter.drawText(footer5Rect, Qt::AlignHCenter | Qt::AlignVCenter, footer5);

    painter.end();
}
