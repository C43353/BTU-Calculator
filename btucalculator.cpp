#include "btucalculator.h"
#include "./ui_btucalculator.h"
#include <QFile>
#include <QTextStream>
#include <QIntValidator>
#include <QDoubleValidator>

BTUCalculator::BTUCalculator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BTUCalculator)
{
    ui->setupUi(this);

    // Setup integer only QLineEdit
    QIntValidator *intValidator = new QIntValidator(INT_MIN, INT_MAX, this);
    ui->occupantLineEdit->setValidator(intValidator);

    // Setup float (numeric) only QLineEdit
    QDoubleValidator *doubleValidator = new QDoubleValidator(-1e100, 1e100, 100, this);
    doubleValidator->setNotation(QDoubleValidator::StandardNotation);
    ui->lengthLineEdit->setValidator(doubleValidator);
    ui->widthLineEdit->setValidator(doubleValidator);
    ui->heightLineEdit->setValidator(doubleValidator);
    ui->northLineEdit->setValidator(doubleValidator);
    ui->eastLineEdit->setValidator(doubleValidator);
    ui->southLineEdit->setValidator(doubleValidator);
    ui->westLineEdit->setValidator(doubleValidator);
    ui->equipmentLineEdit->setValidator(doubleValidator);
    ui->lightingLineEdit->setValidator(doubleValidator);
    ui->peakcoolLineEdit->setValidator(doubleValidator);
    ui->coolcapacityLineEdit->setValidator(doubleValidator);
    ui->wallareaLineEdit->setValidator(doubleValidator);
    ui->windowareaLineEdit->setValidator(doubleValidator);
    ui->ceilingareaLineEdit->setValidator(doubleValidator);
    ui->floorareaLineEdit->setValidator(doubleValidator);
    ui->targettempLineEdit->setValidator(doubleValidator);
    ui->externaltempLineEdit->setValidator(doubleValidator);
    ui->ventilationLineEdit->setValidator(doubleValidator);
    ui->leakageLineEdit->setValidator(doubleValidator);
    ui->peakheatLineEdit->setValidator(doubleValidator);
    ui->heatcapacityLineEdit->setValidator(doubleValidator);

    // Setup dropdown boxes
    ui->lightingCombo->addItem("Incandescent", 4.25);
    ui->lightingCombo->addItem("Florescent", 2.8);
    ui->lightingCombo->addItem("LED", 2);

    ui->wallCombo->addItem("Solid Brick Wall", 2);
    ui->wallCombo->addItem("Cavity Wall (No Insulation)", 1.5);
    ui->wallCombo->addItem("Cavity Wall (Insulated)", 0.55);
    ui->wallCombo->addItem("Modern Insulated Wall", 0.3);
    ui->wallCombo->addItem("Timber Frame Wall (with Insulation)", 0.3);

    ui->windowCombo->addItem("Single Glazed", 5.8);
    ui->windowCombo->addItem("Double Glazed (old, air filled)", 3);
    ui->windowCombo->addItem("Double Glazed (modern, low-e)", 1.6);
    ui->windowCombo->addItem("Triple Glazed", 1);

    ui->ceilingCombo->addItem("Uninsulated Loft", 3);
    ui->ceilingCombo->addItem("Insulated Ceiling", 0.4);
    ui->ceilingCombo->addItem("Modern Insulated Roof", 0.2);
    ui->ceilingCombo->addItem("Warm Flata Roof (Insulated)", 0.25);

    ui->floorCombo->addItem("Uninsulated Solid Floor", 1);
    ui->floorCombo->addItem("Suspended Timber Floor", 1);
    ui->floorCombo->addItem("Insulated Ground-Bearing Slab", 0.25);
    ui->floorCombo->addItem("Retrofit Insulated Timber Floor", 0.3);

    // Set placeholder for every QLineEdit to 0
    QList<QLineEdit *> edits = this->findChildren<QLineEdit *>();
    for (QLineEdit *edit : edits) {
        edit->setPlaceholderText(QString::number(0, 'f', 2));
    }

    // Specific placeholder settings
    ui->targettempLineEdit->setPlaceholderText(QString::number(18, 'f', 2));
    ui->externaltempLineEdit->setPlaceholderText(QString::number(5, 'f', 2));
    ui->ventilationLineEdit->setPlaceholderText(QString::number(4, 'f', 2));
    ui->leakageLineEdit->setPlaceholderText(QString::number(0.5, 'f', 2));
    ui->peakcoolLineEdit->setPlaceholderText(QString::number(10, 'f', 2));
    ui->peakheatLineEdit->setPlaceholderText(QString::number(10, 'f', 2));
    ui->coolcapacityLineEdit->setPlaceholderText(QString::number(12000, 'f', 2));
    ui->heatcapacityLineEdit->setPlaceholderText(QString::number(5000, 'f', 2));

    // Setup placeholder updates on text change
    connect(ui->lengthLineEdit, &QLineEdit::textChanged, this, &BTUCalculator::updatePlaceholders);
    connect(ui->widthLineEdit, &QLineEdit::textChanged, this, &BTUCalculator::updatePlaceholders);
    connect(ui->heightLineEdit, &QLineEdit::textChanged, this, &BTUCalculator::updatePlaceholders);
    connect(ui->northLineEdit, &QLineEdit::textChanged, this, &BTUCalculator::updatePlaceholders);
    connect(ui->eastLineEdit, &QLineEdit::textChanged, this, &BTUCalculator::updatePlaceholders);
    connect(ui->southLineEdit, &QLineEdit::textChanged, this, &BTUCalculator::updatePlaceholders);
    connect(ui->westLineEdit, &QLineEdit::textChanged, this, &BTUCalculator::updatePlaceholders);
}

BTUCalculator::~BTUCalculator()
{
    delete ui;
}

// Function to get line edit values, gets placeholer values if empty
double BTUCalculator::getLineEditValue(QLineEdit* lineEdit) {
    return lineEdit->text().isEmpty() ?
               lineEdit->placeholderText().toDouble() :
               lineEdit->text().toDouble();
}

// Updates placeholders as text is input (use theory to make auto update (no need for calculate button))
void BTUCalculator::updatePlaceholders()
{
    double length = getLineEditValue(ui->lengthLineEdit);
    double width  = getLineEditValue(ui->widthLineEdit);
    double height = getLineEditValue(ui->heightLineEdit);

    double northArea = getLineEditValue(ui->northLineEdit);
    double eastArea = getLineEditValue(ui->eastLineEdit);
    double southArea = getLineEditValue(ui->southLineEdit);
    double westArea = getLineEditValue(ui->westLineEdit);

    double ceilingAreaGuess = length * width;
    double floorAreaGuess = ceilingAreaGuess;
    ui->ceilingareaLineEdit->setPlaceholderText(QString::number(ceilingAreaGuess, 'f', 2));
    ui->floorareaLineEdit->setPlaceholderText(QString::number(floorAreaGuess, 'f', 2));

    double windowAreaGuess = northArea + eastArea + southArea + westArea;
    ui->windowareaLineEdit->setPlaceholderText(QString::number(windowAreaGuess, 'f', 2));

    double wallAreaGuess = (2 * length * height) + (2 * width * height) - windowAreaGuess;
    ui->wallareaLineEdit->setPlaceholderText(QString::number(wallAreaGuess, 'f', 2));
}

// Perform heating and cooling calculations on button press and output to text boxes (Split into functions like original calculator for improved readibility)
void BTUCalculator::on_calculateButton_clicked()
{
    // Obtain variables to use in calcultions
    double length = getLineEditValue(ui->lengthLineEdit);
    double width = getLineEditValue(ui->widthLineEdit);
    double height = getLineEditValue(ui->heightLineEdit);

    double northArea = getLineEditValue(ui->northLineEdit);
    double eastArea = getLineEditValue(ui->eastLineEdit);
    double southArea = getLineEditValue(ui->southLineEdit);
    double westArea = getLineEditValue(ui->westLineEdit);
    double northShade = ui->northCheck->isChecked() ? 1.0 : 1.4;
    double eastShade = ui->eastCheck->isChecked() ? 1.0 : 1.4;
    double southShade = ui->southCheck->isChecked() ? 1.0 : 1.4;
    double westShade = ui->westCheck->isChecked() ? 1.0 : 1.4;

    int occupants = getLineEditValue(ui->occupantLineEdit);
    double equipmentWatt = getLineEditValue(ui->equipmentLineEdit);
    double lightingWatt = getLineEditValue(ui->lightingLineEdit);
    double coolAdjust = getLineEditValue(ui->peakcoolLineEdit);
    double coolCapacity = getLineEditValue(ui->coolcapacityLineEdit);
    double lightingMult = ui->lightingCombo->currentData().toDouble();

    double wallArea = getLineEditValue(ui->wallareaLineEdit);
    double windowArea = getLineEditValue(ui->windowareaLineEdit);
    double ceilingArea = getLineEditValue(ui->ceilingareaLineEdit);
    double floorArea = getLineEditValue(ui->floorareaLineEdit);
    double wallMult = ui->wallCombo->currentData().toDouble();
    double windowMult = ui->windowCombo->currentData().toDouble();
    double ceilingMult = ui->ceilingCombo->currentData().toDouble();
    double floorMult = ui->floorCombo->currentData().toDouble();

    double targetTemp = getLineEditValue(ui->targettempLineEdit);
    double externalTemp = getLineEditValue(ui->externaltempLineEdit);
    double ventRate = getLineEditValue(ui->ventilationLineEdit);
    double leakRate = getLineEditValue(ui->leakageLineEdit);
    double heatAdjust = getLineEditValue(ui->peakheatLineEdit);
    double heatCapacity = getLineEditValue(ui->heatcapacityLineEdit);


    // Calculate Cooling BTU
    double roomArea = length * width * 3.2808399 * 3.2808399; // metres to feet
    double roomBTU = roomArea * 31.25;

    double northWindowBTU = northArea * 164 * northShade;
    double eastWindowBTU = eastArea * 625 * eastShade;
    double southWindowBTU = southArea * 868 * southShade;
    double westWindowBTU = westArea * 625 * westShade;
    double windowCoolBTU = northWindowBTU + eastWindowBTU + southWindowBTU + westWindowBTU;

    double occupantBTU = occupants * 600;

    double equipmentBTU = equipmentWatt * WattBTU;

    double lightingBTU = lightingWatt * lightingMult;

    double totalCoolingBTU = roomBTU + windowCoolBTU + occupantBTU + equipmentBTU + lightingBTU;
    double peakCoolingBTU = totalCoolingBTU * (1 + coolAdjust / 100);

    int numberCoolingUnits = ceil(peakCoolingBTU / coolCapacity);


    // Calculate Heating BTU
    double roomVolume = length * width * height;

    double diffTemp = abs(targetTemp - externalTemp);

    double wallWatt = wallArea * diffTemp * wallMult;
    double wallBTU = wallWatt * WattBTU;
    double windowWatt = windowArea * diffTemp * windowMult;
    double windowHeatBTU = windowWatt * WattBTU;
    double ceilingWatt = ceilingArea * diffTemp * ceilingMult;
    double ceilingBTU = ceilingWatt * WattBTU;
    double floorWatt = floorArea * diffTemp * floorMult;
    double floorBTU = floorWatt * WattBTU;
    double transmissionWatt = wallWatt + windowWatt + ceilingWatt + floorWatt;
    double transmissionBTU = transmissionWatt * WattBTU;

    double ventWatt = AirDensity * AirSpecificHeat * (ventRate / (60*60)) * roomVolume * diffTemp;
    double ventBTU = ventWatt * WattBTU;
    double leakWatt = AirDensity * AirSpecificHeat * (leakRate / (60*60)) * roomVolume * diffTemp;
    double leakBTU = leakWatt * WattBTU;

    double totalHeatingWatt = transmissionWatt + ventWatt + leakWatt;
    double totalHeatingBTU = totalHeatingWatt * WattBTU;
    double peakHeatingWatt = totalHeatingWatt * (1 + heatAdjust / 100);
    double peakHeatingBTU = peakHeatingWatt * WattBTU;

    int numberHeatingUnits = ceil(peakHeatingBTU / heatCapacity);

    // Output results to text boxes
    ui->roomBTUOutput->setText(QString::number(roomBTU, 'f', 2));
    ui->windowBTUOutput->setText(QString::number(windowCoolBTU, 'f', 2));
    ui->occupantBTUOutput->setText(QString::number(occupantBTU, 'f', 2));
    ui->equipmentBTUOutput->setText(QString::number(equipmentBTU, 'f', 2));
    ui->lightingBTUOutput->setText(QString::number(lightingBTU, 'f', 2));
    ui->totalcoolBTUOutput->setText(QString::number(totalCoolingBTU, 'f', 2));
    ui->peakcoolBTUOutput->setText(QString::number(peakCoolingBTU, 'f', 2));
    ui->coolunitsOutput->setText(QString::number(numberCoolingUnits, 'f', 2));

    ui->wallBTUOutput->setText(QString::number(wallBTU, 'f', 2));
    ui->windowBTUOutput_2->setText(QString::number(windowHeatBTU, 'f', 2));
    ui->ceilingBTUOutput->setText(QString::number(ceilingBTU, 'f', 2));
    ui->floorBTUOutput->setText(QString::number(floorBTU, 'f', 2));
    ui->transmissionBTUOutput->setText(QString::number(transmissionBTU, 'f', 2));
    ui->ventilationBTUOutput->setText(QString::number(ventBTU, 'f', 2));
    ui->leakageBTUOutput->setText(QString::number(leakBTU, 'f', 2));
    ui->totalheatBTUOutput->setText(QString::number(totalHeatingBTU, 'f', 2));
    ui->peakheatBTUOutput->setText(QString::number(peakHeatingBTU, 'f', 2));
    ui->heatunitsOutput->setText(QString::number(numberHeatingUnits, 'f', 2));
}

