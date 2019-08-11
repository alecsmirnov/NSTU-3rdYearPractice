#ifndef CROSSDOCK_H
#define CROSSDOCK_H

#include <vector>
#include <string>
#include <cstdint>

struct Product {
	std::uint8_t id;
	std::string name;

	std::vector<std::int16_t>::size_type count;

	double loading_time;
	double unloading_time;
};

class CrossDock {
public:
	CrossDock() : car_capacity(1) {}
	CrossDock(std::vector<std::int16_t>::size_type car_capacity) : car_capacity(car_capacity) {}
	CrossDock(std::vector<std::int16_t>::size_type car_capacity, const std::vector<Product>& products);
	CrossDock(std::vector<std::int16_t>::size_type car_capacity, std::string product_filename);
	CrossDock(std::string car_capacity_filename, std::string product_filename);

	void setCapacity(std::vector<std::int16_t>::size_type car_capacity);
	void setProducts(const std::vector<Product>& products);

	void readCarCapacityFile(std::string filename);
	void readProductsFile(std::string filename);

	void makeCarList();

	void carListBruteForce();

	void printCarList(const std::vector<std::int16_t>& car_list, double time) const;

	~CrossDock();

private:
	void clearProducts();
	void clearCarList();

	double carService(const std::vector<std::int16_t>& car_list, const std::vector<std::int16_t>& new_car_list) const;

private:
	std::vector<Product> products;
	std::vector<std::int16_t> car_list;

	std::vector<std::int16_t>::size_type car_capacity;
};

#endif