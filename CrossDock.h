#ifndef CROSSDOCK_H
#define CROSSDOCK_H

#include <vector>
#include <string>
#include <cstdint>

namespace CDConst {
	static const std::int16_t  EMPTY_PLACE = -1;
	static const std::uint64_t TIME_BASE   = 60;

	static const std::string   NO_FILE;
}

enum class OutputForm {
	NONE, 
	BRIEF,
	FULL
};

struct Product {
	std::uint8_t id;
	std::string name;

	std::vector<std::int16_t>::size_type count;

	std::uint64_t loading_time;
	std::uint64_t unloading_time;
};

struct Delivery {
	std::vector<std::int16_t> car_list;
	std::uint64_t time;
};

class CrossDock {
private:
	using func_ptr = void (CrossDock::*)(std::ostream&, const std::vector<std::int16_t>&, std::uint64_t) const;

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
	void readDeliveryCarList(std::string filename);

	std::string getStrMinutes(std::uint64_t time) const;

	void printNone(std::ostream&, const std::vector<std::int16_t>&, std::uint64_t) const {}
	void printBrief(std::ostream& output_stream, const std::vector<std::int16_t>& car_list, std::uint64_t time) const;
	void printFull(std::ostream& output_stream, const std::vector<std::int16_t>& car_list, std::uint64_t time) const;

	Delivery findOptimalOrder(std::ostream& output_stream, OutputForm output_form, std::string delivery_filename = CDConst::NO_FILE);

	~CrossDock();

private:
	void clearProducts();
	void clearCarList();
	void clearDeliveryCarList();

	std::uint64_t carService(const std::vector<std::int16_t>& car_list, const std::vector<std::int16_t>& new_car_list) const;

	void makeCarList();
	Delivery bruteForceCarList(std::ostream& output_stream, func_ptr print_func);

private:
	std::vector<Product> products;
	std::vector<std::int16_t> car_list;
	std::vector<std::int16_t> delivery_car_list;

	std::vector<std::int16_t>::size_type car_capacity;
};

inline std::string CrossDock::getStrMinutes(std::uint64_t time) const {
	return std::to_string(time / CDConst::TIME_BASE) + "." + std::to_string(time % CDConst::TIME_BASE);
}

#endif