// main.cpp
#include <iostream>
#include "BTUCalculator.h"

using namespace std;

int main() {
	BTUCalculator btuCalculator;

	btuCalculator.inputRoomDetails();
	btuCalculator.inputWindowDetails();
	btuCalculator.inputSurfaceDetails();

	btuCalculator.inputOccupancyDetails();

	btuCalculator.inputApplianceDetails();
	btuCalculator.inputLightingDetails();

	btuCalculator.inputTemperatureDetails();
	btuCalculator.inputAirFlows();

	btuCalculator.inputPeakCoolingDetails();
	btuCalculator.inputCoolingRequirements();
	btuCalculator.inputPeakHeatingDetails();
	btuCalculator.inputHeatingRequirements();

	btuCalculator.calculateHeatingBTU();
	btuCalculator.calculateCoolingBTU();

	btuCalculator.displayResults();

	cout << "Press Enter to exit...";
	cin.ignore();
	cin.get();

	return 0;
}