#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <set>

struct Planet;

using PlanetMap = std::unordered_map<std::string, Planet>;

struct Planet {

    Planet *orbiting = nullptr;

    std::vector<Planet *> satellites;

    std::string name;
};

using StepPlanet = std::pair<Planet*,std::size_t>;

int main() {
    std::ifstream file_stream("input.txt");

    if (!file_stream.is_open()) {
        std::cout << "failed to open file" << std::endl;
        return 1;
    }

    std::string line;
    PlanetMap map;

    while (std::getline(file_stream, line)) {
        std::string planet_one;
        std::string planet_two;
        std::string *working = &planet_one;

        for (std::size_t char_index = 0; char_index < line.size();
             ++char_index) {
            char c = line[char_index];

            if (c == ')') {
                working = &planet_two;
            } else {
                *working += c;
            }
        }

        auto& planet_one_iter = map[planet_one];
        auto& planet_two_iter = map[planet_two];

        planet_one_iter.satellites.push_back(&planet_two_iter);
        planet_two_iter.orbiting = &planet_one_iter;
    }

    std::size_t total_orbits = 0;

    for (auto &[name, data] : map) {
        data.name = name;
        Planet* current = data.orbiting;

        while (current != nullptr) {
            total_orbits++;

            current = current->orbiting;
        }
    }

    std::cout << "total orbits: " << total_orbits << std::endl;

    auto &you = map["YOU"];
    auto &san = map["SAN"];

    std::set<Planet*> visited;
    Planet* current = you.orbiting;
    Planet* common = nullptr;
    std::size_t steps = 0;

    while (current != nullptr) {
        visited.insert(current);

        current = current->orbiting;
    }

    current = san.orbiting;

    while (current != nullptr) {
        if (visited.find(current) != visited.end()) {
            common = current;
            break;
        }

        ++steps;
        current = current->orbiting;
    }

    if (common == nullptr) {
        std::cout << "no common planet found" << std::endl;
        return 1;
    }

    current = you.orbiting;

    while (current != common) {
        ++steps;
        current = current->orbiting;
    }

    std::cout << "total steps: " << steps << std::endl;

    return 0;
}