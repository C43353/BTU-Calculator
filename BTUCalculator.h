// BTUCalculator.h

#ifndef BTUCALCULATOR_H
#define BTUCALCULATOR_H

class BTUCalculator {
public:
	void inputRoomDetails();
	void inputWindowDetails();
	void inputOccupancyDetails();
	void inputApplianceDetails();
	void inputLightingDetails();
	void inputPeakCoolingDetails();
	void inputCoolingRequirements();
	void calculateCoolingBTU();

	void inputSurfaceDetails();
	void inputTemperatureDetails();
	void inputAirFlows();
	void inputPeakHeatingDetails();
	void inputHeatingRequirements();
	void calculateHeatingBTU();

	void displayResults();

private:
	const double WattBTU = 3.412; // 1 watt = 3.412 BTU/hr
	const double AirDensity = 1.225; // kg/m^3
	const double AirSpecificHeat = 1.006; // J/kg.K

	double roomLength;
	double roomWidth;
	double roomHeight;

	double windowAreaNorth;
	double windowAreaEast;
	double windowAreaSouth;
	double windowAreaWest;

	bool shadedNorth;
	bool shadedEast;
	bool shadedSouth;
	bool shadedWest;

	int numberOfOccupants;

	int equipmentWatts;
	int lightingWatts;
	int lightingType; // 1 for incandescent, 2 for fluorescent, 3 for LED

	double peakCoolingLoad;

	double capacityCooling;

	int wallMaterialType; // 1=Solid Brick Wall, 2=Cavity Wall (No Insulation), 3=Cavity Wall (Insulated), 4=Modern Insulated Wall, 5=Timber Frame Wall (with insulation)
	int windowMaterialType; // 1=Single Glazed, 2=Double Glazed (old, air filled), 3=Double Glazed (modern, low-e), 4=Triple Glazed
	int ceilingMaterialType; // 1=Uninsulated Loft, 2=Insulated Ceiling, 3=Modern Insulated Roof, 4=Warm Flat Roof (Insulated)
	int floorMaterialType; // 1=Uninsulated Solid Floor, 2=Suspended Timber Floor, 3=Insulated Ground-Bearing Slab, 4=Retrofit Insulated Timber Floor

	double temperatureDifference;

	double ventilationRate;
	double leakageRate;

	double peakHeatingLoad;

	double capacityHeating;


	double roomBTU;
	double windowBTU;
	double occupantBTU;
	double equipmentBTU;
	double lightingBTU;
	double totalCoolingBTU;
	double peakCoolingBTU;
	double numberCoolingUnits;

	double wallWatts;
	double windowWatts;
	double ceilingWatts;
	double floorWatts;
	double transmissionWatts;
	double ventilationWatts;
	double leakageWatts;
	double totalHeatingWatts;
	double peakHeatingWatts;
	double numberHeatingUnits;


};

#endif // BTUCALCULATOR_H
