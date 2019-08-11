#include "CrossDock.h"

#include <iostream>
#include <fstream>
#include <algorithm>

#define EMPTY_PRODUCT {0, "", 0, 0, 0}
#define EMPTY_PLACE -1

CrossDock::CrossDock(std::vector<std::int16_t>::size_type car_capacity, const std::vector<Product>& products) {
	this->car_capacity = car_capacity;
	this->products = products;
}

CrossDock::CrossDock(std::vector<std::int16_t>::size_type car_capacity, std::string product_filename) {
	this->car_capacity = car_capacity;

	readProductsFile(product_filename);
}

CrossDock::CrossDock(std::string car_capacity_filename, std::string product_filename) {
	readCarCapacityFile(product_filename);
	readProductsFile(product_filename);
}

void CrossDock::setCapacity(std::vector<std::int16_t>::size_type car_capacity) {
	this->car_capacity = car_capacity;
}

void CrossDock::setProducts(const std::vector<Product>& products) {
	this->products = products;
}

void CrossDock::readCarCapacityFile(std::string filename) {
	std::ifstream infile(filename);

	infile >> car_capacity;

	infile.close();
}

void CrossDock::readProductsFile(std::string filename) {
	clearProducts();

	std::ifstream infile(filename);

	Product product = EMPTY_PRODUCT;
	while (infile >> product.name >> product.count >> product.loading_time >> product.unloading_time) {
		products.push_back(product);
		++product.id;
	}

	infile.close();
}

void CrossDock::makeCarList() {
	clearCarList();

	for (auto const& product : products) {
		std::vector<std::int16_t>::size_type free_places = car_capacity < product.count ?
			product.count % car_capacity : car_capacity - product.count;

		for (std::vector<std::int16_t>::size_type i = 0; i != product.count; ++i)
			car_list.push_back(product.id);

		for (std::vector<std::int16_t>::size_type i = 0; i != free_places; ++i)
			car_list.push_back(EMPTY_PLACE);
	}
}

void CrossDock::carListBruteForce() {
	std::vector<std::int16_t> new_car_list(car_list);

	do {
		printCarList(new_car_list, carService(car_list, new_car_list));
	} while (std::next_permutation(new_car_list.begin(), new_car_list.end()));
}

void CrossDock::printCarList(const std::vector<std::int16_t>& car_list, double time) const {
	for (std::vector<std::int16_t>::size_type i = 0; i != car_list.size(); ++i) {
		if (car_list[i] == EMPTY_PLACE)
			std::cout << "-";
		else
			std::cout << car_list[i];

		if ((i + 1) % car_capacity == 0)
			std::cout << " | ";
	}

	std::cout << "Time: " << time << std::endl;
}

CrossDock::~CrossDock() {
	clearProducts();
	clearCarList();
}

void CrossDock::clearProducts() {
	std::vector<Product>().swap(products);
}

void CrossDock::clearCarList() {
	std::vector<std::int16_t>().swap(car_list);
}

double CrossDock::carService(const std::vector<std::int16_t>& car_list, const std::vector<std::int16_t>& new_car_list) const {
	double time = 0;

	for (std::vector<std::int16_t>::size_type i = 0; i != car_list.size(); ++i)
		if (car_list[i] != new_car_list[i] && new_car_list[i] != EMPTY_PLACE)
			time += products[new_car_list[i]].unloading_time + products[new_car_list[i]].loading_time;

	return time;
}