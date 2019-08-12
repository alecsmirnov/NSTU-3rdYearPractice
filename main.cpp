#include <iostream>

#include "CrossDocking.h"

int main(int argc, char* argv[]) {	
	std::string car_capacity_filename = "input/car_capacity.txt";
	std::string products_filename	  = "input/products.txt";
	std::string delivery_filename	  = "input/delivery.txt";

	CrossDocking::Controller cross_dock(car_capacity_filename, products_filename);

	auto delivery = cross_dock.findOptimalOrder(std::cout, CrossDocking::OutputForm::BRIEF, delivery_filename);
	//auto delivery = cross_dock.findOptimalOrder(std::cout, CrossDocking::OutputForm::BRIEF);

	std::cout << std::endl;
	cross_dock.printFull(std::cout, delivery.car_list, delivery.time);

	return 0;
}