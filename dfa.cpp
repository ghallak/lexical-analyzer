#include "dfa.h"

#include <vector>
#include <utility>
#include <memory>
#include <queue>
#include <numeric>
#include <algorithm>
#include <iostream>

DFA::DFA(const NFA& nfa)
{
	using state_closure_pair = std::pair<State*, eps_closure_type>;

	states.push_back(std::make_unique<State>());
	auto q0 = nfa.eps_closure(nfa.start_state());
	auto d0 = states.back().get();
	auto p0 = make_pair(d0, q0);

	head = d0;

	std::vector<state_closure_pair> dfa;
	dfa.push_back(p0);

	std::queue<state_closure_pair> work_list;
	work_list.push(p0);

	while (!work_list.empty())
	{
		auto p = work_list.front();
		auto d = p.first;
		auto q = p.second;

		// if the set that corresponds to the DFA state contains the accepting
		// state of the NFA then the DFA state is an accepting state
		if (q[nfa.accepting_state()]) tails.push_back(d);

		work_list.pop();

		for (auto c : nfa.alphabet())
		{
			auto states_count = nfa.states_count();

			// iterate through each state from q and see if it has
			// transition through character c.
			// if it has, then compute the epsilon closure to the state
			// that results from the transition.

			auto delta = [&nfa, states_count](eps_closure_type closure, char c)
			{
				std::vector<int> states;
				for (int i = 0; i < states_count; ++i)
				{
					if (!closure[i]) continue;

					// transition (state number, char)
					auto next_state = nfa.transition(i, c);
					if (next_state != -1)
					{
						states.push_back(next_state);
					}
				}
				return states;
			};

			// return a vector of epsilon closures for the given states
			auto epsilon_closures = [&nfa](std::vector<int> states)
			{
				std::vector<eps_closure_type> closures;
				for (auto state : states)
				{
					closures.push_back(nfa.eps_closure(state));
				}
				return closures;
			};

			// calculate the union of a set of closures
			auto closures_union = [states_count]
				(std::vector<eps_closure_type> closures)
				{
					return std::accumulate(
						closures.begin(), closures.end(),
						eps_closure_type(states_count),
						[](eps_closure_type closure1, eps_closure_type closure2)
						{
							eps_closure_type result(closure1.size());
							for (std::size_t i = 0; i < result.size(); ++i)
							{
								result[i] = closure1[i] | closure2[i];
							}
							return result;
						});
				};

			// returns true if two boolean vectors have the same size
			// and are equal element-wise
			auto cmp_bool_vector = [](std::vector<bool> v1, std::vector<bool> v2)
			{
				if (v1.size() != v2.size()) return false;

				for (std::size_t i = 0; i < v1.size(); ++i)
				{
					if (v1[i] != v2[i]) return false;
				}
				return true;
			};

			auto all_zeros = [](std::vector<bool> v)
			{
				for (std::size_t i = 0; i < v.size(); ++i)
				{
					if (v[i]) return false;
				}
				return true;
			};

			auto t = closures_union(epsilon_closures(delta(q, c)));

			if (all_zeros(t)) continue;

			auto dd = std::find_if(dfa.begin(), dfa.end(),
			                      [&t, cmp_bool_vector](state_closure_pair p)
			                      {
			                      	return cmp_bool_vector(p.second, t);
			                      });

			if (dd == dfa.end())
			{
				states.push_back(std::make_unique<State>());
				auto new_state = states.back().get();
				dfa.push_back(make_pair(new_state, t));
				work_list.push(make_pair(new_state, t));
				d->adjacent.push_back(std::make_pair(c, new_state));
			}
			else
			{
				d->adjacent.push_back(std::make_pair(c, dd->first));
			}
		}
	}

	for (int i = 0; i < (int)states.size(); ++i)
	{
		states[i]->state_number = i;
	}
}

void DFA::print() const
{
	for (int i = 0; i < (int)states.size(); ++i)
	{
		auto p = states[i].get();
		for (auto s : p->adjacent)
		{
			std::cout << "from state #" << p->state_number
			          << " through: " << (s.first == '\0' ? 'E' : s.first)
			          << " to state #" << s.second->state_number
			          << '\n';
		}
		std::cout << "--------------\n\n";
	}
}
