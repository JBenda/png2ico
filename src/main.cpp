#include <filesystem>
#include <iostream>
#include <vector>
#include <string>

#include "api.hpp"

namespace fs = std::filesystem;

int main(const int _argc, const char** _argv) {
	if (_argc != 3) {
		std::cout << "Usage: [Path to Png] [Path to ico]\n";
		return 0;
	}

	fs::path src(_argv[1]), dst(_argv[2]);

	std::vector<std::string> errorStack = ConvertToICO(src, dst, Options::Overwrite);
	if (!errorStack.empty()) {
		std::cout << "Failed to Convert to ICO: \n";
		for (const std::string& str : errorStack) {
			std::cout << str << '\n';
		}
	} else {
		std::cout << "Success\n";
	}
	return 0;
}

