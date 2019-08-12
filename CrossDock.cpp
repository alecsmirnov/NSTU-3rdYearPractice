#include "CrossDock.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>

static const Product  EMPTY_PRODUCT  = {0, "", 0, 0, 0};
static const Delivery EMPTY_DELIVERY = {{}, 0};

CrossDock::CrossDock(std::vector<std::int16_t>::size_type car_capacity, const std::vector<Product>& products) {
	this->car_capacity = car_capacity;
	this->products = products;
}

CrossDock::CrossDock(std::vector<std::int16_t>::size_type car_capacity, std::string product_filename) {
	this->car_capacity = car_capacity;

	readProductsFile(product_filename);
}

CrossDock::CrossDock(std::string car_capacity_filename, std::string product_filename) {
	readCarCapacityFile(car_capacity_filename);
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

	car_capacity = 1;
	if (infile)
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

void CrossDock::readDeliveryCarList(std::string filename) {
	clearDeliveryCarList();

	std::ifstream infile(filename);

	std::string line;
	while (std::getline(infile, line)) {
		std::stringstream sstream(line);

		std::int16_t product_count = 0;
		std::int16_t product_id = 0;
		std::vector<std::int16_t>::size_type free_places = car_capacity;
		while (sstream >> product_count) {
			for (std::int16_t i = 0; i != product_count; ++i)
				delivery_car_list.push_back(product_id);

			free_places -= product_count;
			++product_id;
		}

		for (std::vector<std::int16_t>::size_type i = 0; i != free_places; ++i)
			delivery_car_list.push_back(CDConst::EMPTY_PLACE);
	}

	infile.close();
}

void CrossDock::printBrief(std::ostream& output_stream, const std::vector<std::int16_t>& car_list, std::uint64_t time) const {
	for (std::vector<std::int16_t>::size_type i = 0; i != car_list.size(); ++i) {
		if (car_list[i] != CDConst::EMPTY_PLACE)
			output_stream << car_list[i] << " ";
		else
			output_stream << "- ";

		if ((i + 1) % car_capacity == 0)
			output_stream << " | ";
	}

	output_stream << getStrMinutes(time) << " min" << std::endl;
}

void CrossDock::printFull(std::ostream& output_stream, const std::vector<std::int16_t>& car_list, std::uint64_t time) const {
	std::vector<Product> car_products(products);
	
	output_stream << "      | ";
	for (std::vector<Product>::size_type i = 0; i != products.size(); ++i)
		output_stream << products[i].name << std::string(std::to_string(car_products[i].count).length(), ' ') << " | ";
	output_stream << std::endl;

	output_stream << "------|";
	std::vector<Product>::size_type col_len = 8;
	for (std::vector<Product>::size_type i = 0; i != products.size(); ++i) {
		col_len = products[i].name.length() + std::to_string(car_products[i].count).length() + 2;
		output_stream << std::string(col_len, '-') << "|";
	}
	output_stream << std::endl;

	for (std::vector<Product>::size_type i = 0; i != car_products.size(); ++i)
		car_products[i].count = 0;

	for (std::vector<std::int16_t>::size_type i = 0; i != car_list.size(); ++i) {
		if ((i + 1) % car_capacity == 0) {
			output_stream << "Car " << i / car_capacity << " | ";

			for (std::vector<Product>::size_type j = 0; j != car_products.size(); ++j)
				output_stream << std::string(car_products[j].name.length(), ' ') << car_products[j].count << " | ";

			for (std::vector<Product>::size_type j = 0; j != car_products.size(); ++j)
				car_products[j].count = 0;

			output_stream << std::endl;
		}
		else
			if (car_list[i] != CDConst::EMPTY_PLACE)
				++car_products[car_list[i]].count;
	}

	output_stream << "\nDelivery time = " << getStrMinutes(time) << " min" << std::endl << std::endl;
}

Delivery CrossDock::findOptimalOrder(std::ostream& output_stream, OutputForm output_form, std::string delivery_filename) {
	Delivery delivery;

	if (delivery_filename != CDConst::NO_FILE)
		readDeliveryCarList(delivery_filename);

	if (!products.empty()) {
		makeCarList();

		func_ptr print_func = nullptr;
		switch (output_form) {
			case OutputForm::NONE:  print_func = &CrossDock::printNone;  break;
			case OutputForm::BRIEF: print_func = &CrossDock::printBrief; break;
			case OutputForm::FULL:  print_func = &CrossDock::printFull;  break;
		}

		auto begin = std::chrono::steady_clock::now();

		delivery = bruteForceCarList(output_stream, print_func);

		auto end = std::chrono::steady_clock::now();
		auto sec = std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();

		output_stream << "\nElapsed time = " << getStrMinutes(sec) << " min" << std::endl;
	}

	return delivery;
}

CrossDock::~CrossDock() {
	clearProducts();
	clearCarList();
	clearDeliveryCarList();
}

void CrossDock::clearProducts() {
	std::vector<Product>().swap(products);
}

void CrossDock::clearCarList() {
	std::vector<std::int16_t>().swap(car_list);
}

void CrossDock::clearDeliveryCarList() {
	std::vector<std::int16_t>().swap(delivery_car_list);
}

std::uint64_t CrossDock::carService(const std::vector<std::int16_t>& car_list, const std::vector<std::int16_t>& new_car_list) const {
	std::uint64_t time = 0;

	for (std::vector<std::int16_t>::size_type i = 0; i != car_list.size(); ++i)
		if (car_list[i] != new_car_list[i] && new_car_list[i] != CDConst::EMPTY_PLACE)
			time += products[new_car_list[i]].unloading_time + products[new_car_list[i]].loading_time;

	return time;
}

void CrossDock::makeCarList() {
	clearCarList();

	for (auto const& product : products) {
		std::vector<std::int16_t>::size_type free_places = car_capacity - (car_capacity < product.count ?
														   product.count % car_capacity : product.count);

		for (std::vector<std::int16_t>::size_type i = 0; i != product.count; ++i)
			car_list.push_back(product.id);

		for (std::vector<std::int16_t>::size_type i = 0; i != free_places; ++i)
			car_list.push_back(CDConst::EMPTY_PLACE);
	}
}

Delivery CrossDock::bruteForceCarList(std::ostream& output_stream, func_ptr print_func) {
	Delivery delivery = EMPTY_DELIVERY;

	std::uint64_t delivery_time = 0;
	std::vector<std::int16_t> new_car_list(car_list);

	do {
		delivery_time = carService(car_list, new_car_list);

		if (new_car_list == delivery_car_list)
			delivery = {new_car_list, delivery_time};

		(this->*print_func)(output_stream, new_car_list, delivery_time);
	} while (std::next_permutation(new_car_list.begin(), new_car_list.end()));

	return delivery;
}