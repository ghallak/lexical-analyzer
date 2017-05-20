#include "finite_automaton.h"

#include <algorithm>
#include <unordered_set>
#include <iostream>

const std::vector<FiniteAutomaton::symbol_type> & FiniteAutomaton::alphabet() const
{
	if (_alphabet.empty())
	{
		std::unordered_set<symbol_type> symbolset;
		for (std::size_t i = 0; i < states.size(); ++i)
		{
			for (const auto& p : states[i]->transitions())
			{
				if (!p.symbol().is_epsilon()) symbolset.insert(p.symbol());
			}
		}
		_alphabet.assign(symbolset.begin(), symbolset.end());
	}
	return _alphabet;
}

int FiniteAutomaton::transition(int state_id, const symbol_type & symbol) const
{
	auto it = std::find_if(states[state_id]->transitions().begin(),
	                       states[state_id]->transitions().end(),
	                       [symbol](auto trans)
	                       { return trans.symbol() == symbol; });

	return it == states[state_id]->transitions().end() ?
		-1 :
		it->state()->state_id();
}

void FiniteAutomaton::print() const
{
	for (std::size_t i = 0; i < states.size(); ++i)
	{
		auto p = states[i].get();
		for (const auto& trans : p->transitions())
		{
			std::cout << "from state #" << p->state_id()
			          << " through: " << (trans.symbol().is_epsilon() ? "epsilon" : trans.symbol().to_string())
			          << " to state #" << trans.state()->state_id()
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
