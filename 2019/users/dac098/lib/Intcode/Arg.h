#pragma once

#include <vector>

namespace Intcode {

	enum class ArgType {
		GET,
		SET
	};

	struct Arg {};

	using ArgList = std::vector<Arg>;



}