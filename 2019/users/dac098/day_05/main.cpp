#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <functional>
#include <map>
#include <stdexcept>
#include <cmath>
#include <memory>

template <typename T>
void printVector(std::ostream& out, const std::vector<T>& list, std::string delimiter) {
	for (std::size_t list_index = 0; list_index < list.size(); ++list_index) {
		if (list_index + 1 == list.size()) {
			out << list[list_index];
		}
		else {
			out << list[list_index] << delimiter;
		}
	}
}

namespace Intcode {

	struct EFailedSettingMemory : std::runtime_error {
		EFailedSettingMemory(int pos, int value) :
			std::runtime_error("failed to set memory location. location: " + std::to_string(pos) + " value: " + std::to_string(value))
		{}
	};

	class Process;

	// declarations

	struct Arg {

		int value;

		Arg() : value(0) {};
		Arg(int v) : value(v) {}

		friend std::ostream& operator<<(std::ostream& out, const Arg& a) {
			out << a.value;
			return out;
		}

	};

	enum class ArgType {
		GET,
		SET
	};

	using ArgList = std::vector<Arg>;

	using Executer = std::function<void(ArgList&, Process&)>;

	using OPID_t = int;

	struct Opcode {

		OPID_t id = 0;

		unsigned arg_len = 0;
		std::vector<ArgType> args;

		Executer exec;

		Opcode(OPID_t i, std::vector<ArgType> a, Executer e) :
			id(i), args(a), exec(e) {}
	};

	struct VM {
		std::map<OPID_t, Opcode> opcodes;

		VM();

		bool addOpcode(Opcode op);
		bool addOpcode(OPID_t id, std::vector<ArgType> args, Executer exec);

		void exec(const std::vector<int>& s);
	};

	class Instructions {
	public:
		using GetValueResult = std::pair<int, bool>;

		Instructions();
		Instructions(const std::vector<int>& script);

		bool validPointer();

		GetValueResult getValue(bool inc);
		GetValueResult getValue(int pos);
		GetValueResult getValue(std::size_t pos);

		bool setValue(std::size_t pos, int value);
		bool setValue(int pos, int value);

		std::size_t getPointer();

		bool setPointer(std::size_t pos);
		bool setPointer(int pos);

		friend std::ostream& operator<<(std::ostream& out, const Instructions& i) {
			printVector(out, i.memory, ",");

			return out;
		}

	private:

		std::size_t pointer = 0;

		std::vector<int> memory;
	};

	class Process {
	public:

		Process(VM& v, Instructions s) : 
			vm(&v), script(s) 
		{}

		void run();

		auto getValue(int pos) {
			return script.getValue(pos);
		}
		auto getPointer() {
			return script.getPointer();
		}

		template <typename T>
		auto setValue(T pos, int value) {
			return script.setValue(pos, value);
		}

		template <typename T>
		auto setPointer(T pos) {
			return script.setPointer(pos);
		}

		int requestInput();

		template <typename T>
		void print(T d);

		template <typename T, typename... Args>
		void print(T d, Args... args);

		void haltExecution() {
			halt = true;
		}

	private:

		int getMode(int opcode, unsigned pos);

		bool halt = false;

		VM* vm = nullptr;
		Instructions script;
	};
}

// VM

Intcode::VM::VM() {};

bool Intcode::VM::addOpcode(Opcode op) {
	auto check = opcodes.find(op.id);

	if (check == opcodes.end()) {
		auto [iter, success] = opcodes.insert(
			std::make_pair(op.id, op)
		);

		return success;
	}

	return false;
}
bool Intcode::VM::addOpcode(OPID_t id, std::vector<ArgType> args, Executer exec) {
	auto check = opcodes.find(id);

	if (check == opcodes.end()) {
		auto [iter, success] = opcodes.insert(
			std::make_pair(id, Opcode(id, args, exec))
		);

		return success;
	}

	return false;
}

void Intcode::VM::exec(const std::vector<int>& s) {
	Instructions script(s);

	Process p(*this, script);

	p.run();
}

// Instructions

Intcode::Instructions::Instructions() {};
Intcode::Instructions::Instructions(const std::vector<int>& script) :
	memory(script)
{}

bool Intcode::Instructions::validPointer() { return pointer < memory.size(); }

Intcode::Instructions::GetValueResult Intcode::Instructions::getValue(bool inc = false) {
	if (validPointer())
		return getValue(inc ? pointer++ : pointer);
	else
		return GetValueResult(0, false);
}
Intcode::Instructions::GetValueResult Intcode::Instructions::getValue(int pos) {
	if (pos < 0) {
		return GetValueResult(0, false);
	}

	std::size_t cast = static_cast<std::size_t>(pos);

	return getValue(cast);
}
Intcode::Instructions::GetValueResult Intcode::Instructions::getValue(std::size_t pos) {
	if (pos >= memory.size()) {
		return GetValueResult(0, false);
	}

	return GetValueResult(
		memory[pos],
		true
	);
}

std::size_t Intcode::Instructions::getPointer() {
	return pointer;
}

bool Intcode::Instructions::setPointer(std::size_t pos) {
	if (pos >= memory.size()) {
		return false;
	}

	pointer = pos;

	return true;
}
bool Intcode::Instructions::setPointer(int pos) {
	if (pos < 0) {
		return false;
	}

	auto cast = static_cast<std::size_t>(pos);

	return setPointer(cast);
}

bool Intcode::Instructions::setValue(std::size_t pos, int value) {
	if (pos >= memory.size()) {
		return false;
	}

	memory[pos] = value;

	return true;
}
bool Intcode::Instructions::setValue(int pos, int value) {
	if (pos < 0) {
		return false;
	}

	std::size_t cast = static_cast<std::size_t>(pos);

	return setValue(cast, value);
}

// Process

void Intcode::Process::run() {
	while (!halt && script.validPointer()) {
		auto [instruction,success] = script.getValue(true);

		if (!success) {
			throw std::runtime_error("failed to retrieve opcode");
		}

		int opcode = instruction % 100;

		auto opcode_iter = vm->opcodes.find(opcode);

		if (opcode_iter == vm->opcodes.end()) {
			throw std::runtime_error("unknown opcode given: " + std::to_string(opcode));
		}

		auto [op_id,op_data] = *opcode_iter;

		ArgList args;

		for (unsigned count = 0; count < op_data.args.size(); ++count) {
			auto [cached_value,good] = script.getValue(true);

			if (!good) {
				throw std::runtime_error("failed to retrieve argument");
			}

			Arg arg;

			if (op_data.args[count] == ArgType::SET) {
				arg.value = cached_value;
				
				args.push_back(arg);

				continue;
			}

			int mode = getMode(instruction,count + 3);

			switch (mode) {
			case 0:
			{
				// position mode, fetch value at position
				auto [fetched_value, success] = script.getValue(cached_value);

				if (!success) {
					throw std::runtime_error("failed to retrieve value for argument");
				}

				arg.value = fetched_value;
				break;
			}
			case 1:
				// immediate mode, do nothing
				arg.value = cached_value;
				break;
			default:
				throw std::runtime_error("unknown argument mode: " + std::to_string(mode));
			}

			args.push_back(arg);
		}

		op_data.exec(args,*this);
	}
}

int Intcode::Process::requestInput() {
	std::string line;

	std::cout << '>' << std::flush;

	if (!std::getline(std::cin, line)) {
		throw std::runtime_error("failed to retrieve input");
	}

	return std::stoi(line);
}

template<typename T>
void Intcode::Process::print(T d) {
	std::cout << d << std::endl;
}

template<typename T, typename... Args>
void Intcode::Process::print(T d,Args... args) {
	std::cout << d;
	print(std::forward<Args>(args)...);
}

int Intcode::Process::getMode(int opcode, unsigned pos) {
	auto cast = static_cast<double>(pos);
	auto modulus = std::pow(10.0, cast);
	auto divide = std::pow(10.0, cast - 1);

	return static_cast<int>(std::fmod(static_cast<double>(opcode), modulus) / divide);
}

Intcode::Opcode addOp(
	1,
	{
		Intcode::ArgType::GET,
		Intcode::ArgType::GET,
		Intcode::ArgType::SET
	},
	[](Intcode::ArgList& args, Intcode::Process& process) {
		auto value = args[0].value + args[1].value;

		if (!process.setValue(args[2].value,value))
			throw Intcode::EFailedSettingMemory(args[2].value,value);
	}
);

Intcode::Opcode multOp(
	2,
	{
		Intcode::ArgType::GET,
		Intcode::ArgType::GET,
		Intcode::ArgType::SET
	},
	[](Intcode::ArgList& args, Intcode::Process& process) {
		auto value = args[0].value * args[1].value;

		if (!process.setValue(args[2].value,value))
			throw Intcode::EFailedSettingMemory(args[2].value,value);
	}
);

Intcode::Opcode inputOp(
	3,
	{Intcode::ArgType::SET},
	[](Intcode::ArgList& args, Intcode::Process& process) {
		int given = process.requestInput();

		if (!process.setValue(args[0].value, given))
			throw Intcode::EFailedSettingMemory(args[0].value,given);
	}
);

Intcode::Opcode outputOp(
	4,
	{Intcode::ArgType::GET},
	[](Intcode::ArgList& args, Intcode::Process& process) {
		process.print(args[0].value);
	}
);

Intcode::Opcode jumpIfTrueOp(
	5,
	{
		Intcode::ArgType::GET,
		Intcode::ArgType::GET
	},
	[](Intcode::ArgList& args, Intcode::Process& process) {
		if (args[0].value != 0) {
			process.setPointer(args[1].value);
		}
	}
);

Intcode::Opcode jumpIfFalseOp(
	6,
	{
		Intcode::ArgType::GET,
		Intcode::ArgType::GET
	},
	[](Intcode::ArgList& args, Intcode::Process& process) {
		if (args[0].value == 0) {
			process.setPointer(args[1].value);
		}
	}
);

Intcode::Opcode lessThanOp(
	7,
	{
		Intcode::ArgType::GET,
		Intcode::ArgType::GET,
		Intcode::ArgType::SET
	},
	[](Intcode::ArgList& args, Intcode::Process& process) {
		process.setValue(
			args[2].value,
			args[0].value < args[1].value ? 1 : 0
		);
	}
);

Intcode::Opcode equalsOp(
	8,
	{
		Intcode::ArgType::GET,
		Intcode::ArgType::GET,
		Intcode::ArgType::SET
	},
	[](Intcode::ArgList& args, Intcode::Process& process) {
		process.setValue(
			args[2].value,
			args[0].value == args[1].value ? 1 : 0
		);
	}
);

Intcode::Opcode haltOp(
	99,
	{},
	[](Intcode::ArgList& args, Intcode::Process& process) {
		process.haltExecution();
	}
);

int main() {
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

	if (collecting.size() > 0) {
		try {
			intcodes.push_back(std::stoi(collecting));
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

	intcodes.shrink_to_fit();

	Intcode::VM vm;

	vm.addOpcode(addOp);
	vm.addOpcode(multOp);
	vm.addOpcode(inputOp);
	vm.addOpcode(outputOp);
	vm.addOpcode(jumpIfTrueOp);
	vm.addOpcode(jumpIfFalseOp);
	vm.addOpcode(lessThanOp);
	vm.addOpcode(equalsOp);
	vm.addOpcode(haltOp);

	vm.exec(intcodes);

	return 0;
}