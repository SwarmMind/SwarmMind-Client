#include <string>
#include <fstream>
#include <iostream>

#include <game/Settings.h>

Settings::Settings(const std::string& _filename) :
	filename{ _filename } {
}
Settings::~Settings() {
	save();
}

void Settings::read() {
	std::ifstream ifs{ filename, std::ios::binary };

	if (ifs.fail()) return;

	ifs >> hostname;
	ifs >> port;
	ifs >> username;
}
void Settings::save() const {
	std::ofstream ofs{ filename, std::ios::binary | std::ios::trunc };

	ofs << hostname << '\n' << port << '\n' << username;
}