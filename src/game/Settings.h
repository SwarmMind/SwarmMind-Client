#pragma once

#include <string>
#include <cstdint>
#include <fstream>
#include <iostream>

class Settings {
protected:
	std::string filename;
public:
	std::string hostname;
	uint16_t port;

	Settings(const std::string& _filename = "settings.txt") : filename{ _filename } {
	}
	Settings(const Settings& rhs) = delete;
	~Settings() {
		save();
	}

	void read() {
		std::ifstream ifs{ filename, std::ios::binary };

		if (ifs.fail()) {
			hostname = "localhost";
			port = 3000;
			return;
		}
		ifs >> hostname;
		ifs >> port;
	}
	void save() {
		std::ofstream ofs{ filename, std::ios::binary | std::ios::trunc };

		ofs << hostname << '\n' << port;
	}
};