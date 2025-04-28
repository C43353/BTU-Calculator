// BTUCalculator.cpp

#include <iostream>
#include "BTUCalculator.h"

using namespace std;

void BTUCalculator::inputRoomDetails() {
	cout << "Enter room length (meters): ";
	cin >> roomLength;
	cout << "Enter room width (meters): ";
	cin >> roomWidth;
	cout << "Enter room height (meters): ";
	cin >> roomHeight;
}

void BTUCalculator::inputWindowDetails() {
	cout << "Enter North window area (meters squared): ";
	cin >> windowAreaNorth;
	cout << "Shaded? (1=yes, 0=no): ";
	cin >> shadedNorth;
	cout << "Enter East window area (meters squared): ";
	cin >> windowAreaEast;
	cout << "Shaded? (1=yes, 0=no): ";
	cin >> shadedEast;
	cout << "Enter South window area (meters squared): ";
	cin >> windowAreaSouth;
	cout << "Shaded? (1=yes, 0=no): ";
	cin >> shadedSouth;
	cout << "Enter West window area (meters squared): ";
	cin >> windowAreaWest;
	cout << "Shaded? (1=yes, 0=no): ";
	cin >> shadedWest;
}

void BTUCalculator::inputSurfaceDetails() {
	cout << "Enter wall material type (1=Solid Brick Wall, 2=Cavity Wall (No Insulation), 3=Cavity Wall (Insulated), 4=Modern Insulated Wall, 5=Timber Frame Wall (with insulation): ";
	cin >> wallMaterialType;
	cout << "Enter window material type (1=Single Glazed, 2=Double Glazed (old, air filled), 3=Double Glazed (modern, low-e), 4=Triple Glazed): ";
	cin >> windowMaterialType;
	cout << "Enter ceiling material type (1=Uninsulated Loft, 2=Insulated Ceiling, 3=Modern Insulated Roof, 4=Warm Flat Roof (Insulated): ";
	cin >> ceilingMaterialType;
	cout << "Enter floor material type (1=Uninsulated Solid Floor, 2=Suspended Timber Floor, 3=Insulated Ground-Bearing Slab, 4=Retrofit Insulated Timber Floor): ";
	cin >> floorMaterialType;
}

void BTUCalculator::inputOccupancyDetails() {
	cout << "Enter standard number of occupants: ";
	cin >> numberOfOccupants;
}

void BTUCalculator::inputApplianceDetails() {
	cout << "Enter total equipment wattage: ";
	cin >> equipmentWatts;
}

void BTUCalculator::inputLightingDetails() {
	cout << "Enter total lighting wattage: ";
	cin >> lightingWatts;
	cout << "Enter lighting type (1=incandescent, 2=florescent, 3=LED): ";
	cin >> lightingType;
}

void BTUCalculator::inputTemperatureDetails() {
	cout << "Enter the internal-external temperature difference (Celsius): ";
	cin >> temperatureDifference;
}

void BTUCalculator::inputAirFlows() {
	cout << "Enter the ventilation (desired) air replacements per hour (Lookup suitable value depending on room type): ";
	cin >> ventilationRate;
	cout << "Enter the leakage (unwanted) air replacements per hour (0.5-2 depending on insulation/age of building): ";
	cin >> leakageRate;
}

void BTUCalculator::inputPeakCoolingDetails() {
	cout << "Enter the peak cooling load adjustment (%): ";
	cin >> peakCoolingLoad;
}

void BTUCalculator::inputPeakHeatingDetails() {
	cout << "Enter the peak heating load adjustment (%): ";
	cin >> peakHeatingLoad;
}

void BTUCalculator::inputCoolingRequirements() {
	cout << "Enter cooling capacity per  Unit (BTU): ";
	cin >> capacityCooling;
}

void BTUCalculator::inputHeatingRequirements() {
	cout << "Enter heating capacity per Unit (BTU): ";
	cin >> capacityHeating;
}

void BTUCalculator::calculateCoolingBTU() {
	double roomArea = roomLength * roomWidth * 3.2808399 * 3.2808399; // metres to feet
	roomBTU = roomArea * 31.25;

	double northWindowBTU = windowAreaNorth * 164 * (shadedNorth ? 1 : 1.4);
	double eastWindowBTU = windowAreaEast * 625 * (shadedEast ? 1 : 1.4);
	double southWindowBTU = windowAreaSouth * 868 * (shadedSouth ? 1 : 1.4);
	double westWindowBTU = windowAreaWest * 625 * (shadedWest ? 1 : 1.4);
	windowBTU = northWindowBTU + eastWindowBTU + southWindowBTU + westWindowBTU;

	occupantBTU = numberOfOccupants * 600;

	equipmentBTU = equipmentWatts * WattBTU;

	lightingBTU = lightingWatts * (lightingType == 1 ? 4.25 : lightingType == 2 ? 2.8 : lightingType == 3 ? 2 : 0);

	totalCoolingBTU = roomBTU + windowBTU + occupantBTU + equipmentBTU + lightingBTU;
	peakCoolingBTU = totalCoolingBTU * (1 + peakCoolingLoad / 100);

	numberCoolingUnits = ceil(peakCoolingBTU / capacityCooling);
}

void BTUCalculator::calculateHeatingBTU() {
	double roomVolume = roomLength * roomWidth * roomHeight;
	double windowSurfaceArea = windowAreaNorth + windowAreaEast + windowAreaSouth + windowAreaWest;
	double wallSurfaceArea = (2 * roomLength * roomHeight) + (2 * roomWidth * roomHeight) - windowSurfaceArea;
	double ceilingSurfaceArea = roomLength * roomWidth;
	double floorSurfaceArea = ceilingSurfaceArea;

	wallWatts = wallSurfaceArea * temperatureDifference;
	switch (wallMaterialType) {
	case 1: wallWatts = wallWatts * 2; break; // Solid Brick Wall
	case 2: wallWatts = wallWatts * 1.5; break; // Cavity Wall (No Insulation)
	case 3: wallWatts = wallWatts * 0.55; break; // Cavity Wall (Insulated)
	case 4: wallWatts = wallWatts * 0.3; break; // Modern Insulated Wall
	case 5: wallWatts = wallWatts * 0.3; break; // Timber Frame Wall (with insulation)
	}

	windowWatts = windowSurfaceArea * temperatureDifference;
	switch (windowMaterialType) {
	case 1: windowWatts = windowWatts * 5.8; break; // Single Glazed
	case 2: windowWatts = windowWatts * 3; break; // Double Glazed (old, air filled)
	case 3: windowWatts = windowWatts * 1.6; break; // Double Glazed (modern, low-e)
	case 4: windowWatts = windowWatts * 1; break; // Triple Glazed
	}

	ceilingWatts = ceilingSurfaceArea * temperatureDifference;
	switch (ceilingMaterialType) {
	case 1: ceilingWatts = ceilingWatts * 3; break; // Uninsulated Loft
	case 2: ceilingWatts = ceilingWatts * 0.4; break; // Insulated Ceiling
	case 3: ceilingWatts = ceilingWatts * 0.2; break; // Modern Insulated Roof
	case 4: ceilingWatts = ceilingWatts * 0.25; break; // Warm Flat Roof (Insulated)
	}

	floorWatts = floorSurfaceArea * temperatureDifference;
	switch (floorMaterialType) {
	case 1: floorWatts = floorWatts * 1; break; // Uninsulated Solid Floor
	case 2: floorWatts = floorWatts * 1; break; // Suspended Timber Floor
	case 3: floorWatts = floorWatts * 0.25; break; // Insulated Ground-Bearing Slab
	case 4: floorWatts = floorWatts * 0.3; break; // Retrofit Insulated Timber Floor
	}

	transmissionWatts = wallWatts + windowWatts + ceilingWatts + floorWatts;

	ventilationWatts = AirDensity * AirSpecificHeat * (ventilationRate / (60 * 60)) * roomVolume * temperatureDifference;
	leakageWatts = AirDensity * AirSpecificHeat * (leakageRate / (60 * 60)) * roomVolume * temperatureDifference;

	totalHeatingWatts = transmissionWatts + ventilationWatts + leakageWatts;
	peakHeatingWatts = totalHeatingWatts * (1 + peakHeatingLoad / 100);
	double totalHeatingBTU = peakHeatingWatts * WattBTU;
	double peakHeatingBTU = peakHeatingWatts * WattBTU;

	numberHeatingUnits = ceil(peakHeatingBTU / capacityHeating);
}

void BTUCalculator::displayResults() {
	cout << endl;
	cout << "Cooling Information:" << endl;
	cout << "Room BTU: " << roomBTU << endl;
	cout << "Window BTU: " << windowBTU << endl;
	cout << "Occupant BTU: " << occupantBTU << endl;
	cout << "Equipment BTU: " << equipmentBTU << endl;
	cout << "Lighting BTU: " << lightingBTU << endl;
	cout << "Total Cooling BTU: " << totalCoolingBTU << endl;
	cout << "Peak Cooling BTU: " << peakCoolingBTU << endl;
	cout << "Number of cooling units required: " << numberCoolingUnits << endl;
	cout << endl;
	cout << "Heating Information:" << endl;
	cout << "Wall BTU: " << wallWatts * WattBTU << endl;
	cout << "Window BTU: " << windowWatts * WattBTU << endl;
	cout << "Ceiling BTU: " << ceilingWatts * WattBTU << endl;
	cout << "Floor BTU: " << floorWatts * WattBTU << endl;
	cout << "Transmission BTU: " << transmissionWatts * WattBTU << endl;
	cout << "Ventilation BTU: " << ventilationWatts * WattBTU << endl;
	cout << "Leakage BTU: " << leakageWatts * WattBTU << endl;
	cout << "Total Heating BTU: " << totalHeatingWatts * WattBTU << endl;
	cout << "Peak Heating BTU: " << peakHeatingWatts * WattBTU << endl;
	cout << "Number of heating units required: " << numberHeatingUnits << endl;
	cout << endl;
}
