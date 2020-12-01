#include <iostream>
#include <vector>
#include <string>

bool hasConsecutiveValues(const std::string& password, std::size_t start, std::size_t len) {
	if (start == password.size() - 1 && len > 1) {
		return false;
	}

	char checking = password.at(start);
	std::size_t count = 1;

	for (; count < len && start + count < password.size(); ++count) {
		if (password[start + count] != checking) {
			return false;
		}
	}

	return count == len;
}

std::size_t totalConsecutiveValues(const std::string& password, std::size_t start) {
	if (start == password.size() - 1) {
		return 1;
	}

	char checking = password[start];
	std::size_t count = 1;

	for (; start + count < password.size(); ++count) {
		if (checking != password[start + count]) {
			return count;
		}
	}

	return count;
}

int main() {
	int current = 356261;
	int max = 846303;
	std::vector<int> valid_passwords;
	bool p1 = false;

	while (current <= max) {
		std::string password = std::to_string(current);
		char prev = 0;
		bool found_large_consecutive = false;
		bool found_consecutive = false;
		bool no_decreasing = true;
		std::size_t skip_consecutive = 0;

		for (std::size_t char_i = 0; char_i < password.size(); ++char_i) {
			char c = password[char_i];

			if (c < prev) {
				no_decreasing = false;
				break;
			}
			else {
				prev = c;
			}

			if (p1) {
				if (!found_consecutive) {
					found_consecutive = hasConsecutiveValues(password, char_i, 2);
				}
			}
			else {
				if (skip_consecutive == 0) {
					auto consecutive_values = totalConsecutiveValues(password, char_i);

					if (consecutive_values > 1) {
						if (consecutive_values > 2) {
							found_large_consecutive = false;
						}
						else {
							found_consecutive = true;
						}

						skip_consecutive = consecutive_values - 1;
					}
				}
				else {
					--skip_consecutive;
				}
			}
		}

		if (found_consecutive && no_decreasing) {
			valid_passwords.push_back(current);
		}

		++current;
	}

	std::cout << "total valid passwords: " << valid_passwords.size() << std::endl;

	return 0;
}