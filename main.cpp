#include <iostream>

#include "CrossDocking.h"

int main(int argc, char* argv[]) {	
	std::string car_capacity_filename   = "input/car_capacity.txt";
	std::string products_filename	    = "input/products.txt";
	std::string delivery_table_filename	= "input/delivery_table.txt";

	CrossDocking::Controller controller(car_capacity_filename, products_filename);

	auto delivery = controller.findOptimalOrder(std::cout, CrossDocking::OutputForm::FULL, delivery_table_filename);

	if (!controller.getCarList().empty()) {
		std::cout << std::endl << "Initial order:" << std::endl;

		controller.printFull(std::cout, controller.getCarList(), 0);
	}

	if (controller.getProducts().size() <= 1)
		std::cout << "No products to re-sort!" << std::endl;
	else 
		if (!delivery.car_list.empty()) {
			std::cout << "Optimal order:" << std::endl;

			controller.printFull(std::cout, delivery.car_list, delivery.time);
		}
		else
			std::cout << "Optimal order not found!" << std::endl;

	return 0;
}