#include <iostream>

#include "CrossDock.h"

int main(int argc, char* argv[]) {	
	CrossDock cross_dock(5, "products.txt");
	cross_dock.makeCarList();
	cross_dock.carListBruteForce();

	return 0;
}