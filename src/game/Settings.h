#pragma once

#include <string>
#include <cstdint>

class Settings {
protected:
	std::string filename;
public:
	std::string hostname = "localhost";
	uint16_t port = 3000;
	std::string username = "Player";

	Settings(const std::string& _filename = "settings.txt");
	Settings(const Settings& rhs) = delete;
	~Settings();

	void read();
	void save() const;
};