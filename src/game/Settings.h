#pragma once

#include <string>
#include <cstdint>

class Settings {
protected:
	std::string m_filename;
public:
	std::string m_hostname = "localhost";
	uint16_t m_port = 3000;
	std::string m_username = "Player";

	Settings(const std::string& filename = "settings.txt");
	Settings(const Settings& rhs) = delete;
	~Settings();

	void read();
	void save() const;
};