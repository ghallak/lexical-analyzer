#include "finite_automaton.h"

#include <algorithm>
#include <unordered_set>
#include <iostream>

std::vector<char> FiniteAutomaton::alphabet() const
{
	if (_alphabet.empty())
	{
		std::unordered_set<char> charset;
		for (std::size_t i = 0; i < states.size(); ++i)
		{
			for (auto p : states[i]->transitions)
			{
				if (p.first != '\0') charset.insert(p.first);
			}
		}
		_alphabet.assign(charset.begin(), charset.end());
	}
	return _alphabet;
}

int FiniteAutomaton::transition(int state_number, char c) const
{
	auto it = std::find_if(states[state_number]->transitions.begin(),
	                       states[state_number]->transitions.end(),
	                       [c](std::pair<char, State*> trans)
	                       { return trans.first == c; });

	return it == states[state_number]->transitions.end() ?
		-1 :
		it->second->state_number;
}

void FiniteAutomaton::print() const
{
	for (std::size_t i = 0; i < states.size(); ++i)
	{
		auto p = states[i].get();
		for (auto s : p->transitions)
		{
			std::cout << "from state #" << p->state_number
			          << " through: " << (s.first == '\0' ? 'E' : s.first)
			          << " to state #" << s.second->state_number
			          << '\n';
		}
		std::cout << "--------------\n\n";
	}
}
