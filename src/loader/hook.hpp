#pragma once

#include <vector>


struct Hook {

	constexpr auto operator<=>(const Hook& h) const {
		return priority <=> h.priority;
	}


	void* target;
	void* detour;

	int priority;

};

struct HooksNode {

	constexpr void* getNextTarget() {

		if (targetIndex-- == 0) {
			targetIndex = hooks.size() - 1;
		}

		return hooks[targetIndex].target;

	}

	constexpr void appendHook(Hook hook) {

		if (!hooks.empty()) {
			hook.target = hooks.back().detour;
		}

		hooks.push_back(hook);

	}


	size_t targetIndex = 0;

	std::vector<Hook> hooks;

};
