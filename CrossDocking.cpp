#include "CrossDocking.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>

namespace CrossDocking {

static const Product  EMPTY_PRODUCT  = {0, "", 0, 0, 0};
static const Delivery EMPTY_DELIVERY = {{}, std::numeric_limits<std::uint64_t>::max()};

Controller::Controller(std::vector<std::int16_t>::size_type car_capacity, const std::vector<Product>& products) {
	this->car_capacity = car_capacity;
	this->products = products;
}

Controller::Controller(std::vector<std::int16_t>::size_type car_capacity, std::string product_filename) {
	this->car_capacity = car_capacity;

	readProductsFile(product_filename);
}

Controller::Controller(std::string car_capacity_filename, std::string product_filename) {
	readCarCapacityFile(car_capacity_filename);
	readProductsFile(product_filename);
}

void Controller::setCapacity(std::vector<std::int16_t>::size_type car_capacity) {
	this->car_capacity = car_capacity;
}

void Controller::setProducts(const std::vector<Product>& products) {
	this->products = products;
}

std::vector<std::int16_t>::size_type Controller::getCarCapacity() const {
	return car_capacity;
}

std::vector<std::int16_t>::size_type Controller::getCarsCount() const {
	return cars_count;
}

void Controller::readCarCapacityFile(std::string filename) {
	std::ifstream infile(filename);

	car_capacity = 1;
	if (infile)
		infile >> car_capacity;		

	infile.close();
}

void Controller::readProductsFile(std::string filename) {
	clearProducts();

	std::ifstream infile(filename);

	auto product = EMPTY_PRODUCT;
	while (infile >> product.name >> product.count >> product.loading_time >> product.unloading_time) {
		products.push_back(product);
		++product.id;
	}

	infile.close();
}

void Controller::printBrief(std::ostream& output_stream, const std::vector<std::int16_t>& car_list, std::uint64_t time) const {
	for (std::vector<std::int16_t>::size_type i = 0; i != car_list.size(); ++i) {
		if (car_list[i] != EMPTY_PLACE)
			output_stream << car_list[i] << " ";
		else
			output_stream << "- ";

		if ((i + 1) % car_capacity == 0)
			output_stream << " | ";
	}

	output_stream << getStrMinutes(time) << " min" << std::endl;
}

void Controller::printFull(std::ostream& output_stream, const std::vector<std::int16_t>& car_list, std::uint64_t time) const {
	auto car_products(products);
	
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

	for (std::vector<std::int16_t>::size_type i = 0; i != cars_count; ++i) {
		for (std::vector<Product>::size_type j = 0; j != car_products.size(); ++j)
			car_products[j].count = 0;

		auto car = getCar(car_list, i);
		for (std::vector<std::int16_t>::size_type j = 0; j != car.size(); ++j)
			if (car[j] != EMPTY_PLACE)
				++car_products[car[j]].count;

		output_stream << "Car " << i << " | ";
		for (std::vector<Product>::size_type j = 0; j != car_products.size(); ++j)
			output_stream << std::string(car_products[j].name.length(), ' ') << car_products[j].count << " | ";

		output_stream << std::endl;
	}

	output_stream << "Delivery time = " << getStrMinutes(time) << " min" << std::endl << std::endl;
}

Delivery Controller::findOptimalOrder(std::ostream& output_stream, OutputForm output_form, const std::vector<std::vector<std::int16_t>>& delivery_table) {
	Delivery delivery = {car_list, 0};

	if (1 < products.size()) {
		convertDeliveryTableToList(delivery_table);

		delivery = findOptimalOrder(output_stream, output_form);
	}

	return delivery;
}

Delivery Controller::findOptimalOrder(std::ostream& output_stream, OutputForm output_form, std::string delivery_table_filename) {
	Delivery delivery = {car_list, 0};

	if (1 < products.size()) {
		readDeliveryCarList(delivery_table_filename);

		delivery = findOptimalOrder(output_stream, output_form);
	}

	return delivery;
}

Controller::~Controller() {
	clearProducts();
	clearCarList();
	clearDeliveryCarList();
}

void Controller::clearProducts() {
	std::vector<Product>().swap(products);
}

void Controller::clearCarList() {
	std::vector<std::int16_t>().swap(car_list);
}

void Controller::clearDeliveryCarList() {
	std::vector<std::int16_t>().swap(delivery_car_list);
}

void Controller::convertDeliveryTableToList(const std::vector<std::vector<std::int16_t>>& delivery_table) {
	clearDeliveryCarList();

	for (std::vector<std::int16_t>::size_type i = 0; i != delivery_table.size(); ++i) {
		auto free_places = car_capacity;

		for (std::int16_t j = 0; j != delivery_table[i].size(); ++j) {
			for (std::int16_t k = 0; k != delivery_table[i][j]; ++k)
				delivery_car_list.push_back(j);

			free_places -= delivery_table[i][j];
		}

		for (std::vector<std::int16_t>::size_type j = 0; j != free_places; ++j)
			delivery_car_list.push_back(EMPTY_PLACE);
	}
}

void Controller::readDeliveryCarList(std::string filename) {
	clearDeliveryCarList();

	std::ifstream infile(filename);

	std::string line;
	while (std::getline(infile, line)) {
		std::stringstream sstream(line);

		std::int16_t product_count = 0;
		std::int16_t product_id = 0;
		auto free_places = car_capacity;
		while (sstream >> product_count) {
			for (std::int16_t i = 0; i != product_count; ++i)
				delivery_car_list.push_back(product_id);

			free_places -= product_count;
			++product_id;
		}

		for (std::vector<std::int16_t>::size_type i = 0; i != free_places; ++i)
			delivery_car_list.push_back(EMPTY_PLACE);
	}

	infile.close();
}

std::vector<std::int16_t> Controller::getCar(const std::vector<std::int16_t>& car_list, std::vector<std::int16_t>::size_type car_id) const {
	auto begin = car_list.begin() + car_id * car_capacity;
	auto end = car_list.begin() + (car_id + 1) * car_capacity;

	return std::vector<std::int16_t>(begin, end);
}

std::uint64_t Controller::carService(const std::vector<std::int16_t>& car_list, const std::vector<std::int16_t>& new_car_list) const {
	std::uint64_t time = 0;
	
	for (std::vector<std::int16_t>::size_type i = 0; i != car_list.size(); ++i)
		if (car_list[i] != new_car_list[i] && car_list[i] != EMPTY_PLACE)
			if (car_list[i] != EMPTY_PLACE && new_car_list[i] != EMPTY_PLACE)
				time += products[car_list[i]].unloading_time + products[new_car_list[i]].loading_time;

	return time;
}

void Controller::makeCarList() {
	clearCarList();

	for (auto const& product : products) {
		auto free_places = car_capacity - (car_capacity < product.count ?
						   product.count % car_capacity : product.count);

		for (std::vector<std::int16_t>::size_type i = 0; i != product.count; ++i)
			car_list.push_back(product.id);

		for (std::vector<std::int16_t>::size_type i = 0; i != free_places; ++i)
			car_list.push_back(EMPTY_PLACE);
	}

	cars_count = car_list.size() / car_capacity;
}

Delivery Controller::bruteForceCarList(std::ostream& output_stream, func_ptr print_func) const {
	auto delivery = EMPTY_DELIVERY;

	auto new_car_list(car_list);

	std::vector<std::vector<std::int16_t>> delivery_cars;
	for (std::vector<std::int16_t>::size_type i = 0; i != cars_count; ++i)
		delivery_cars.push_back(getCar(delivery_car_list, i));

	do {
		auto cur_delivery_time = carService(car_list, new_car_list);

		bool equal = true;
		for (std::vector<std::int16_t>::size_type i = 0; i != delivery_cars.size() && equal; ++i) {
			auto unloading_car = getCar(new_car_list, i);

			equal = false;
			for (std::vector<std::int16_t>::size_type j = 0; j != delivery_cars.size() && !equal; ++j)
				if (std::is_permutation(unloading_car.begin(), unloading_car.end(), delivery_cars[j].begin()))
					equal = true;
		}
		
		if (equal && cur_delivery_time < delivery.time)
			delivery = {new_car_list, cur_delivery_time};

		(this->*print_func)(output_stream, new_car_list, cur_delivery_time);
	} while (std::next_permutation(new_car_list.begin(), new_car_list.end()));

	return delivery;
}

Delivery Controller::findOptimalOrder(std::ostream& output_stream, OutputForm output_form) {
	makeCarList();

	func_ptr print_func = nullptr;
	switch (output_form) {
		case OutputForm::NONE:  print_func = &Controller::printNone;  break;
		case OutputForm::BRIEF: print_func = &Controller::printBrief; break;
		case OutputForm::FULL:  print_func = &Controller::printFull;  break;
	}

	auto begin = std::chrono::steady_clock::now();

	auto delivery = bruteForceCarList(output_stream, print_func);

	auto end = std::chrono::steady_clock::now();
	auto sec = std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();

	output_stream << "Elapsed time = " << getStrMinutes(sec) << " min" << std::endl;

	return delivery;
}

}