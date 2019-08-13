#include <iostream>

#include "CrossDocking.h"

int main(int argc, char* argv[]) {	
	std::string car_capacity_filename = "input/car_capacity.txt";
	std::string products_filename	  = "input/products.txt";
	std::string delivery_filename	  = "input/delivery.txt";

	CrossDocking::Controller controller(car_capacity_filename, products_filename);

	auto delivery = controller.findOptimalOrder(std::cout, CrossDocking::OutputForm::BRIEF, delivery_filename);

	//const std::vector<std::vector<std::int16_t>>& delivery_table {{1, 1, 0}, 
	//															  {0, 0, 2}, 
	//															  {1, 2, 0}};

	//auto delivery = controller.findOptimalOrder(std::cout, CrossDocking::OutputForm::NONE, delivery_table);

	std::cout << std::endl;
	controller.printFull(std::cout, delivery.car_list, delivery.time);

	return 0;
}