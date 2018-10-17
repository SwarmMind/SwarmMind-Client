#include <string>
#include <fstream>
#include <iostream>

#include <game/Settings.h>

Settings::Settings(const std::string& filename) :
	m_filename{ filename } {
}
Settings::~Settings() {
	save();
}

void Settings::read() {
	std::ifstream ifs{ m_filename, std::ios::binary };

	if (ifs.fail()) return;

	ifs >> m_hostname;
	ifs >> m_port;
	ifs >> m_username;
}
void Settings::save() const {
	std::ofstream ofs{ m_filename, std::ios::binary | std::ios::trunc };

	ofs << m_hostname << '\n' << m_port << '\n' << m_username;
}