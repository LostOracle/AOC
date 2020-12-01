#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <tuple>
#include <limits>

struct Segment {

	char direction = '\0';

	int distance = 0;

	Segment(char dir, int dis) : direction(dir), distance(dis) {}
};

struct Wire {

	short id = 0;

	std::string data;

	std::vector<Segment> path;

};

struct Coordinate {
	int x = 0;
	int y = 0;

	Coordinate() {}
	Coordinate(int x_arg, int y_arg) : x(x_arg), y(y_arg) {}

	Coordinate(const Coordinate& copy) : x(copy.x), y(copy.y) {}

	friend std::ostream& operator<<(std::ostream& out, const Coordinate& coor) {
		out << "x: " << coor.x << " y: " << coor.y;
		return out;
	}
};

using CoordinateSegment = std::map<short, unsigned>;

bool operator<(const Coordinate& a, const Coordinate& b) {
	static auto shift = sizeof(int) * 8;
	long long a_v = (static_cast<long long>(a.x) << shift);
	a_v += a.y;
	long long b_v = (static_cast<long long>(b.x) << shift);
	b_v += b.y;

	return a_v < b_v;
}


using ParseIntResult = std::pair<int, bool>;

ParseIntResult parseInt(const std::string& to_parse) try {
	ParseIntResult rtn;

	rtn.first = std::stoi(to_parse);
	rtn.second = true;

	return rtn;
}
catch (const std::invalid_argument&) {
	return ParseIntResult(0, false);
}
catch (const std::out_of_range&) {
	return ParseIntResult(0, false);
}

template <typename T>
struct Grid {

	using value_t = T;

	struct Column {

		std::vector<value_t> col;

		Grid* parent = nullptr;

		Column(Grid& p) : parent(&p) {}
		Column(Grid& p, std::size_t y_size) : parent(&p) {
			resize(y_size);
		}

		void resize(std::size_t y_size) {
			if (col.size() < y_size) {
				col.reserve(y_size);

				for (std::size_t y = col.size(); y < y_size; ++y) {
					col.push_back(value_t());
				}
			}
		}

		std::size_t size() { return col.size(); }

		value_t& operator[](std::size_t y) {
			if (y >= col.size()) {
				parent->resizeY(y + 1);
			}

			return col[y];
		}
	};

	std::vector<Column> g;

	Grid() {}
	Grid(std::size_t x_size, std::size_t y_size) {
		resize(x_size, y_size);
	}

	void resizeX(std::size_t x_size) {
		std::size_t y_current = g.size() != 0 ? g[0].size() : 0;

		if (g.size() < x_size) {
			g.reserve(x_size);

			for (std::size_t x = g.size(); x < x_size; ++x) {
				g.push_back(Column(*this,y_current));
			}
		}
	}

	void resizeY(std::size_t y_size) {
		if (g.size() != 0) {
			if (g[0].size() < y_size) {
				for (std::size_t x = 0; x < g.size(); ++x) {
					g[x].resize(y_size);
				}
			}
		}
	}

	void resize(std::size_t x_size, std::size_t y_size) {
		std::size_t y_current = g.size() != 0 ? g[0].size() : 0;
		bool resize_y = false;

		if (y_current < y_size) {
			y_current = y_size;
			resize_y = true;
		}

		if (g.size() < x_size) {
			g.reserve(x_size);

			if (resize_y) {
				for (std::size_t x = 0; x < g.size(); ++x) {
					g[x].resize(resize_y);
				}
			}

			for (std::size_t x = g.size(); x < x_size + 1; ++x) {
				g.push_back(Column(*this,y_current));
			}
		}
	}

	std::size_t xSize() { return g.size(); }
	std::size_t ySize() { return g.size() != 0 ? g[0].size() : 0; }

	Column& operator[](std::size_t x) {
		if (x >= g.size()) {
			resizeX(x + 1);
		}

		return g[x];
	}
};

struct Day3 {

	Wire red;
	Wire blue;

	std::map<Coordinate, CoordinateSegment> occupied_spaces;

	std::set<Coordinate> overlapping;

	short cross_value = 0;

	int largest_x = std::numeric_limits<int>::min();
	int smallest_x = std::numeric_limits<int>::max();

	int largest_y = std::numeric_limits<int>::min();
	int smallest_y = std::numeric_limits<int>::max();

	Day3() {
		red.id = 0b1;
		blue.id = 0b10;

		cross_value = 0b11;
	}

	bool processDirection(Wire& wire, char direction, int distance, int& x, int& y, unsigned& steps) {
		Coordinate coor(x,y);
		int count = 0;

		switch (direction) {
		case 'U':
			for (; count < distance; ++count) {
				++coor.y;
				++steps;

				occupied_spaces[coor][wire.id] = steps;

				if (occupied_spaces[coor].size() > 1) {
					overlapping.insert(coor);
				}
			}
			y += distance;
			break;
		case 'D': 
			for (; count < distance; ++count) {
				--coor.y;
				++steps;

				occupied_spaces[coor][wire.id] = steps;

				if (occupied_spaces[coor].size() > 1) {
					overlapping.insert(coor);
				}
			}
			y -= distance;
			break;
		case 'L': 
			for (; count < distance; ++count) {
				--coor.x;
				++steps;

				occupied_spaces[coor][wire.id] = steps;

				if (occupied_spaces[coor].size() > 1) {
					overlapping.insert(coor);
				}
			}
			x -= distance;
			break;
		case 'R': 
			for (; count < distance; ++count) {
				++coor.x;
				++steps;

				occupied_spaces[coor][wire.id] = steps;

				if (occupied_spaces[coor].size() > 1) {
					overlapping.insert(coor);
				}
			}
			x += distance;
			break;
		default: return false;
		}

		if (y > largest_y)
			largest_y = y;

		if (y < smallest_y)
			smallest_y = y;

		if (x > largest_x)
			largest_x = x;

		if (x < smallest_x)
			smallest_x = x;

		wire.path.push_back(Segment(direction, distance));

		return true;
	}

	void mapWireData(Wire& wire) {
		std::string token;
		int x = 0;
		int y = 0;
		unsigned steps = 0;

		char direction = '\0';
		bool collect_direction = true;

		for (std::size_t char_index = 0ull; char_index < wire.data.size(); ++char_index) {
			char c = wire.data[char_index];

			if (c == ',') {
				auto [distance, success] = parseInt(token);

				if (success) {
					processDirection(wire, direction, distance, x, y, steps);
				}

				token.clear();
				collect_direction = true;
			}
			else {
				if (collect_direction) {
					direction = c;
					collect_direction = false;
				}
				else {
					token += c;
				}
			}
		}
	}

	void start() {
		auto input_stream = std::ifstream("input.txt");

		if (!std::getline(input_stream, red.data)) {
			std::cout << "failed to load red wire data from file" << std::endl;

			input_stream.close();

			return;
		}

		if (!std::getline(input_stream, blue.data)) {
			std::cout << "failed to load blue wire data from file" << std::endl;

			input_stream.close();

			return;
		}

		input_stream.close();

		mapWireData(red);
		mapWireData(blue);

		bool p1 = false;

		if (p1) {
			int manhattan = std::numeric_limits<int>::max();
			Coordinate manhattan_coor;

			for (const auto& coor : overlapping) {
				int m_distance = std::abs(coor.x) + std::abs(coor.y);

				if (m_distance < manhattan) {
					manhattan = m_distance;
					manhattan_coor = coor;
				}
			}

			std::cout << "best manhattan distance: " << manhattan << " " << manhattan_coor << std::endl;
		}
		else {
			unsigned smallest_step = std::numeric_limits<unsigned>::max();
			Coordinate step_coor;

			for (const auto& coor : overlapping) {
				unsigned total_steps = 0;

				for (const auto& [id, value] : occupied_spaces[coor]) {
					total_steps += value;
				}

				if (total_steps < smallest_step) {
					smallest_step = total_steps;
					step_coor = coor;
				}
			}

			std::cout << "smallest total steps: " << smallest_step << " " << step_coor << std::endl;
		}
	}

};

int main() {
	Day3 app;

	app.start();

	return 0;
}