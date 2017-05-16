#include "finite_automaton.h"

#include <algorithm>
#include <unordered_set>

std::vector<char> FiniteAutomaton::alphabet() const
{
	// TODO: store the alphabet in private variable to avoid
	// multiple calcultions of the same alphabet

	std::unordered_set<char> charset;
	for (std::size_t i = 0; i < states.size(); ++i)
	{
		for (auto p : states[i]->adjacent)
		{
			if (p.first != '\0') charset.insert(p.first);
		}
	}
	return std::vector<char>(charset.begin(), charset.end());
}

int FiniteAutomaton::transition(int state_number, char c) const
{
	auto it = std::find_if(states[state_number]->adjacent.begin(),
							states[state_number]->adjacent.end(),
							[c](std::pair<char, State*> trans)
							{ return trans.first == c; });

	return it == states[state_number]->adjacent.end() ?
		-1 :
		it->second->state_number;
}
