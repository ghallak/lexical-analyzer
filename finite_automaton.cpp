#include "finite_automaton.h"

#include <algorithm>
#include <unordered_set>
#include <iostream>

const std::vector<FiniteAutomaton::symbol_type> & FiniteAutomaton::alphabet() const
{
	if (_alphabet.empty())
	{
		std::unordered_set<symbol_type> charset;
		for (std::size_t i = 0; i < states.size(); ++i)
		{
			for (const auto& p : states[i]->transitions())
			{
				if (p.symbol() != '\0') charset.insert(p.symbol());
			}
		}
		_alphabet.assign(charset.begin(), charset.end());
	}
	return _alphabet;
}

int FiniteAutomaton::transition(int state_id, symbol_type c) const
{
	auto it = std::find_if(states[state_id]->transitions().begin(),
	                       states[state_id]->transitions().end(),
	                       [c](auto trans)
	                       { return trans.symbol() == c; });

	return it == states[state_id]->transitions().end() ?
		-1 :
		it->state()->state_id();
}

void FiniteAutomaton::print() const
{
	for (std::size_t i = 0; i < states.size(); ++i)
	{
		auto p = states[i].get();
		for (const auto& s : p->transitions())
		{
			std::cout << "from state #" << p->state_id()
			          << " through: " << (s.symbol() == '\0' ? 'E' : s.symbol())
			          << " to state #" << s.state()->state_id()
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
