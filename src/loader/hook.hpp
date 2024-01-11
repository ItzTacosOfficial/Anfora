#pragma once

#include <compare>
#include <vector>
#include <unordered_map>
#include <algorithm>


struct Hook {

	constexpr Hook(void* target, void* function, HMODULE source) : target(target), function(function), source(source) {}


	constexpr auto operator<=>(const Hook& other) const {
		return priority <=> other.priority;
	}


	void* target;
	void* function;

	unsigned int priority;

	HMODULE source;

};

struct HookNode {

	constexpr Hook& next() {

		if (index-- == 0) {
			index = hooks.size() - 1;
		}

		return hooks[index];

	}

	constexpr void append(Hook hook) {

		if (!hooks.empty()) {
			hook.target = hooks.back().function;
		}

		hooks.emplace_back(hook);

	}

	constexpr void sort() {
		std::sort(hooks.begin(), hooks.end());
	}


	std::vector<Hook> hooks;

	size_t index;

};

using HookMap = std::unordered_map<uintptr_t, HookNode>;
