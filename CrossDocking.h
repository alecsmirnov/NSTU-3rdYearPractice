#ifndef CROSSDOCKING_H
#define CROSSDOCKING_H

#include <vector>
#include <string>
#include <cstdint>

namespace CrossDocking {
	static const std::int16_t  EMPTY_PLACE = -1;
	static const std::uint64_t TIME_BASE   = 60;

	enum class OutputForm;
	struct Product;
	struct Delivery;
	class Controller;
}

enum class CrossDocking::OutputForm {
	NONE,
	BRIEF,
	FULL
};

struct CrossDocking::Product {
	std::uint8_t id;
	std::string name;

	std::vector<std::int16_t>::size_type count;

	std::uint64_t loading_time;
	std::uint64_t unloading_time;
};

struct CrossDocking::Delivery {
	std::vector<std::int16_t> car_list;
	std::uint64_t time;
};

class CrossDocking::Controller {
private:
	using func_ptr = void (Controller::*)(std::ostream&, const std::vector<std::int16_t>&, std::uint64_t) const;

public:
	Controller() : car_capacity(1), cars_count(0) {}
	Controller(std::vector<std::int16_t>::size_type car_capacity) : car_capacity(car_capacity), cars_count(0) {}
	Controller(std::vector<std::int16_t>::size_type car_capacity, const std::vector<Product>& products);
	Controller(std::vector<std::int16_t>::size_type car_capacity, std::string product_filename);
	Controller(std::string car_capacity_filename, std::string product_filename);

	void readCarCapacityFile(std::string filename);
	void readProductsFile(std::string filename);

	void setCapacity(std::vector<std::int16_t>::size_type car_capacity);
	void setProducts(const std::vector<Product>& products);

	std::vector<std::int16_t>::size_type getCarCapacity() const;
	std::vector<std::int16_t>::size_type getCarsCount() const;

	std::vector<Product> getProducts() const;
	std::vector<std::int16_t> getCarList() const;
	std::vector<std::int16_t> deliveryCarList() const;

	std::string getStrMinutes(std::uint64_t time) const;

	void printNone(std::ostream&, const std::vector<std::int16_t>&, std::uint64_t) const {}
	void printBrief(std::ostream& output_stream, const std::vector<std::int16_t>& car_list, std::uint64_t time) const;
	void printFull(std::ostream& output_stream, const std::vector<std::int16_t>& car_list, std::uint64_t time) const;

	Delivery findOptimalOrder(std::ostream& output_stream, OutputForm output_form, const std::vector<std::vector<std::int16_t>>& delivery_table);
	Delivery findOptimalOrder(std::ostream& output_stream, OutputForm output_form, std::string delivery_table_filename);

	~Controller();

private:
	void clearProducts();
	void clearCarList();
	void clearDeliveryCarList();

	void convertDeliveryTableToList(const std::vector<std::vector<std::int16_t>>& delivery_table);
	void readDeliveryCarList(std::string filename);

	std::vector<std::int16_t> getCar(const std::vector<std::int16_t>& car_list, std::vector<std::int16_t>::size_type car_id) const;
	std::uint64_t carService(const std::vector<std::int16_t>& car_list, const std::vector<std::int16_t>& new_car_list) const;

	void makeCarList();
	Delivery bruteForceCarList(std::ostream& output_stream, func_ptr print_func) const;

	Delivery findOptimalOrder(std::ostream& output_stream, OutputForm output_form);

private:
	std::vector<Product> products;
	std::vector<std::int16_t> car_list;
	std::vector<std::int16_t> delivery_car_list;

	std::vector<std::int16_t>::size_type car_capacity;
	std::vector<std::int16_t>::size_type cars_count;
};

inline std::string CrossDocking::Controller::getStrMinutes(std::uint64_t time) const {
	return std::to_string(time / TIME_BASE) + "." + std::to_string(time % TIME_BASE);
}

#endif