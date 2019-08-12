#include <iostream>

#include "CrossDock.h"

int main(int argc, char* argv[]) {	
	CrossDock cross_dock("input/car_capacity.txt", "input/products.txt");

	auto delivery = cross_dock.findOptimalOrder(std::cout, OutputForm::BRIEF, "input/delivery.txt");
	//auto delivery = cross_dock.findOptimalOrder(std::cout, OutputForm::BRIEF);

	std::cout << std::endl;
	cross_dock.printFull(std::cout, delivery.car_list, delivery.time);

	return 0;
}