#include <iostream>
#include <string>
#include <fstream>
#include <vector>

inline void checkInRange(const std::vector<int>& list, std::size_t index) {
	if (index >= list.size()) {
		throw std::out_of_range("index is larger than list");
	}
}

int getCode(const std::vector<int>& list, std::size_t index) {
	std::size_t to_get = static_cast<std::size_t>(list.at(index));

	return list.at(to_get);
}

struct OpCodeData {
	int first_value = 0;
	int second_value = 0;
	std::size_t to_set = 0;

	OpCodeData(const std::vector<int>& intcodes, std::size_t code_index) {
		first_value = getCode(intcodes, code_index + 1);
		second_value = getCode(intcodes, code_index + 2);
		std::size_t set_pos = code_index + 3;

		checkInRange(intcodes, set_pos);

		to_set = static_cast<std::size_t>(intcodes[set_pos]);
	}
};

bool processIntcodes(std::vector<int>& intcodes) {
	for (std::size_t code_index = 0; code_index < intcodes.size(); code_index += 4) {
		int opcode = intcodes[code_index];

		if (opcode == 1 || opcode == 2) {
			OpCodeData data(intcodes, code_index);

			if (opcode == 1) {
				intcodes[data.to_set] = data.first_value + data.second_value;
			}
			else {
				intcodes[data.to_set] = data.first_value * data.second_value;
			}
		}
		else if (opcode == 99) {
			return true;
		}
		else {
			std::cout << "unknown opcode: " << opcode << " index: " << code_index << std::endl;
			return false;
		}
	}

	return true;
}

int main(int argc, char** argv) {
	std::ifstream input_stream = std::ifstream("input.txt");

	std::string data;

	if (!std::getline(input_stream, data)) {
		std::cout << "error reading file" << std::endl;
		return 0;
	}

	std::vector<int> intcodes;
	std::size_t current_count = 0;
	std::string collecting;

	intcodes.reserve(10);

	for (std::size_t char_index = 0; char_index < data.size(); ++char_index) {
		if (data[char_index] == ',') {
			try {
				intcodes.push_back(std::stoi(collecting));

				collecting.clear();

				if (++current_count == 10) {
					intcodes.reserve(intcodes.size() + 10);
					current_count = 0;
				}
			}
			catch (const std::invalid_argument&) {
				std::cout << "invalid argument: " << collecting << std::endl;
				return 1;
			}
			catch (const std::out_of_range&) {
				std::cout << "out of range: " << collecting << std::endl;
				return 1;
			}
		}
		else {
			collecting += data[char_index];
		}
	}

	intcodes.shrink_to_fit();
	unsigned max_value = 99;
	int searching = 19690720;

	for (unsigned noun = 0; noun <= max_value; ++noun) {
		for (unsigned verb = 0; verb <= max_value; ++verb) {
			auto intcodes_copy = intcodes;

			intcodes_copy[1] = noun;
			intcodes_copy[2] = verb;

			if (!processIntcodes(intcodes_copy)) {
				return 0;
			}

			if (intcodes_copy[0] == searching) {
				std::cout << "noun: " << noun << " verb: " << verb << std::endl;
				return 0;
			}
		}
	}

	std::cout << "failed to find search value" << std::endl;

	return 0;
}