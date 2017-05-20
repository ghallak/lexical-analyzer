#include "dfa.h"

#include <vector>
#include <utility>
#include <memory>
#include <queue>
#include <numeric>
#include <algorithm>
#include <unordered_map>

DFA::DFA(const NFA & nfa)
{
	using state_closure_pair = std::pair<State*, eps_closure_type>;

	states.emplace_back(std::make_unique<State>());
	auto q0 = nfa.eps_closure(nfa.start_state());
	auto d0 = states.back().get();
	auto p0 = std::make_pair(d0, q0);

	head = d0;

	std::vector<state_closure_pair> dfa;
	dfa.emplace_back(p0);

	std::queue<state_closure_pair> work_list;
	work_list.emplace(p0);

	while (!work_list.empty())
	{
		auto p = work_list.front();
		auto d = p.first;
		auto q = p.second;

		// if the set that corresponds to the DFA state contains the accepting
		// state of the NFA then the DFA state is an accepting state
		if (q[nfa.accepting_state()]) tails.emplace_back(d);

		work_list.pop();

		for (auto symbol : nfa.alphabet())
		{
			auto states_count = nfa.states_count();

			auto delta = [&nfa, states_count](eps_closure_type closure, symbol_type symbol)
			{
				std::vector<int> states;
				for (int i = 0; i < states_count; ++i)
				{
					if (!closure[i]) continue;

					auto next_state = nfa.transition(i, symbol);
					if (next_state != -1)
					{
						states.emplace_back(next_state);
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
					closures.emplace_back(nfa.eps_closure(state));
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

			auto t = closures_union(epsilon_closures(delta(q, symbol)));

			// continue if all elements in t are false
			if (std::all_of(t.begin(), t.end(), [](bool b) { return !b; }))
			{
				continue;
			}

			auto dd = std::find_if(dfa.begin(), dfa.end(),
			                       [&t](state_closure_pair p)
			                       { return p.second == t; });

			if (dd == dfa.end())
			{
				states.emplace_back(std::make_unique<State>());
				auto new_state = states.back().get();
				dfa.emplace_back(new_state, t);
				work_list.emplace(new_state, t);
				d->add_transition(new_state, symbol);
			}
			else
			{
				d->add_transition(dd->first, symbol);
			}
		}
	}

	set_states_ids();
}

void DFA::minimize()
{
	// Initially: partition 0 is for non-accepting states
	//            partition 1 is for acception states
	int parts_count = 2;
	std::vector<int> part(states.size());
	for (const auto& tail : tails)
	{
		part[tail->state_id()] = 1;
	}

	int old_parts_count = -1;
	while (old_parts_count != parts_count)
	{
		old_parts_count = parts_count;

		// Iterate over partitions
		for (int current_part = 0; current_part < parts_count; ++current_part)
		{
			// Iterate over symbols of the alphabet
			for (auto symbol : alphabet())
			{
				std::unordered_map<int, int> to; // from next_part to new partition number

				// Iterate over states inside one partition
				for (std::size_t state_id = 0; state_id < states.size(); ++state_id)
				{
					if (part[state_id] != current_part) continue;

					auto next_state = transition(state_id, symbol);
					auto next_part = next_state == -1 ? -1 : part[next_state];

					if (to.empty())
					{
						to[next_part] = current_part;
					}
					else if (to.find(next_part) == to.end())
					{
						to[next_part] = parts_count++; // new partition number
					}

					part[state_id] = to[next_part];
				}
			}
		}
	}

	update_dfa(part, parts_count);
}

void DFA::update_dfa(const std::vector<int> & part, int parts_count)
{
	std::vector<std::unique_ptr<State>> new_states(parts_count);

	for (std::size_t state_id = 0; state_id < states.size(); ++state_id)
	{
		if (new_states[part[state_id]] == nullptr)
		{
			new_states[part[state_id]] = std::move(states[state_id]);
		}
	}

	for (auto& state : new_states)
	{
		const auto& trans = state->transitions();

		for (std::size_t i = 0; i < trans.size(); ++i)
		{
			auto state_id = trans[i].state()->state_id();

			// If a state belongs to old states (not nullptr), update it
			if (states[state_id] != nullptr)
			{
				state->update_transition(i,
				                         new_states[part[state_id]].get(),
				                         trans[i].symbol());
			}
		}
	}

	states.assign(std::make_move_iterator(new_states.begin()),
	              std::make_move_iterator(new_states.end()));

	set_states_ids();
}
