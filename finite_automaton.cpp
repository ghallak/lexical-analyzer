#include "finite_automaton.h"

#include <algorithm>
#include <unordered_set>
#include <iostream>

const std::vector<char> & FiniteAutomaton::alphabet() const
{
	if (_alphabet.empty())
	{
		std::unordered_set<char> charset;
		for (std::size_t i = 0; i < states.size(); ++i)
		{
			for (const auto& p : states[i]->transitions())
			{
				if (p.first != '\0') charset.insert(p.first);
			}
		}
		_alphabet.assign(charset.begin(), charset.end());
	}
	return _alphabet;
}

int FiniteAutomaton::transition(int state_id, char c) const
{
	auto it = std::find_if(states[state_id]->transitions().begin(),
	                       states[state_id]->transitions().end(),
	                       [c](State::transition_type trans)
	                       { return trans.first == c; });

	return it == states[state_id]->transitions().end() ?
		-1 :
		it->second->state_id();
}

void FiniteAutomaton::print() const
{
	for (std::size_t i = 0; i < states.size(); ++i)
	{
		auto p = states[i].get();
		for (const auto& s : p->transitions())
		{
			std::cout << "from state #" << p->state_id()
			          << " through: " << (s.first == '\0' ? 'E' : s.first)
			          << " to state #" << s.second->state_id()
			          << '\n';
		}
		std::cout << "--------------\n\n";
	}
}

void FiniteAutomaton::set_states_ids()
{
	for (std::size_t i = 0; i < states.size(); ++i)
	{
		states[i]->_state_id = i;
	}
}
