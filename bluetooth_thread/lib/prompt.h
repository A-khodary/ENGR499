#ifndef PROMTP_H
#define PROMTP_H

#include "config.h"
#include <iostream>
#include <string>
#include <sstream>


void print_menu() {
	printf("What to do?\n");
	printf("1. Search for Bluetooth devices\n");
	printf("2. Send data to other devices\n");
	printf("3. Wait for data from other devices\n");
	printf("4. Exit\n");
	printf("Enter your choice (all other input is considered exit): ");
}


int prompt_device(std::string& input, const int& max_num) {
	if (max_num <= 0) {
		return -1;
	}

	std::stringstream ss;
	int choice = 0;

	while(true) {
		printf("Select a device: ");
		getline(std::cin, input);
		ss << input;
		
		if (ss >> choice && (choice >= 0 && choice <= max_num)) {
			// success
			break;
		}
		else {
			// clear the string stream
			ss.clear();
			ss.str("");

			printf("Invalid input, try again!\n");
		}
	}

	return choice;
}

#endif