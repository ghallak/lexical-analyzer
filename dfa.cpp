#include "dfa.h"

#include <vector>
#include <utility>
#include <memory>
#include <queue>
#include <numeric>
#include <algorithm>
#include <unordered_map>

#include "utility.h"

DFA::DFA(const AugmentedRegexTree& tree)
{
	using leaves_set_type = AugmentedRegexTree::leaves_set_type;

	auto create_state = [this]
	{
		states.emplace_back(std::make_unique<State>());
		return states.back().get();
	};

	set_alphabet(tree.labels());

	std::vector<std::pair<State*, leaves_set_type>> dstates;
	std::queue<std::size_t> q;

	dstates.emplace_back(create_state(), tree.firstpos_root());
	q.emplace(0);

	while (!q.empty())
	{
		auto state = dstates[q.front()].first;
		auto leaves = dstates[q.front()].second;
		q.pop();

		for (auto leaf_pos : leaves)
		{
			// TODO: Create special symbol for end of augmented reged (#)
			if (tree.label(leaf_pos).to_string() == "#")
			{
				accept_states.emplace_back(state, tree.leaf_regex_id(leaf_pos));
				break;
			}
		}

		for (const auto& symbol : alphabet())
		{
			leaves_set_type new_leaves;
			for (auto leaf_pos : leaves)
			{
				if (tree.label(leaf_pos) == symbol)
				{
					new_leaves = utility::union_sets(new_leaves,
					                                 tree.followpos(leaf_pos));
				}
			}

			if (new_leaves.empty()) continue;

			auto it = std::find_if(dstates.cbegin(), dstates.cend(),
			                       [new_leaves](auto elem)
			                       { return new_leaves == elem.second; });

			if (it == dstates.end())
			{
				auto new_state = create_state();
				q.emplace(dstates.size());
				dstates.emplace_back(new_state, new_leaves);

				state->add_transition(new_state, symbol);
			}
			else
			{
				state->add_transition(it->first, symbol);
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
	for (const auto& accept_state : accept_states)
	{
		part[accept_state.state->state_id()] = 1;
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
				// From next_part to new partition number
				std::unordered_map<int, int> to;

				// Iterate over states inside one partition
				for (std::size_t state_id = 0;
					 state_id < states.size();
					 ++state_id)
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
