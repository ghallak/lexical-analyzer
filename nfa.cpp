#include "nfa.h"

#include <queue>
#include <algorithm>

NFA::NFA(const symbol_type & symbol)
{
	states.emplace_back(std::make_unique<State>());
	states.emplace_back(std::make_unique<State>());

	head = states.front().get();
	tail = states.back().get();

	head->add_transition(tail, symbol);
}

NFA& NFA::operator+=(NFA&& rhs)
{
	if (empty())
	{
		*this = std::move(rhs);
	}
	else
	{
		tail->add_transition(rhs.head);
		tail = rhs.tail;

		states.insert(states.end(),
		              make_move_iterator(rhs.states.begin()),
		              make_move_iterator(rhs.states.end()));
	}

	return *this;
}

NFA& NFA::operator|=(NFA&& rhs)
{
	if (empty())
	{
		*this = std::move(rhs);
	}
	else
	{
		states.emplace_back(std::make_unique<State>());
		auto new_head = states.back().get();
		states.emplace_back(std::make_unique<State>());
		auto new_tail = states.back().get();

		new_head->add_transition(head);
		new_head->add_transition(rhs.head);

		tail->add_transition(new_tail);
		rhs.tail->add_transition(new_tail);

		head = new_head;
		tail = new_tail;

		states.insert(states.end(),
		              std::make_move_iterator(rhs.states.begin()),
		              std::make_move_iterator(rhs.states.end()));
	}

	return *this;
}

NFA&& NFA::operator*() &&
{
	states.emplace_back(std::make_unique<State>());
	auto new_head = states.back().get();
	states.emplace_back(std::make_unique<State>());
	auto new_tail = states.back().get();

	tail->add_transition(head);
	tail->add_transition(new_tail);
	new_head->add_transition(head);
	new_head->add_transition(new_tail);

	head = new_head;
	tail = new_tail;

	// TODO: check if this is correct
	return std::move(*this);
}

std::vector<bool> NFA::eps_closure(int state) const
{
	std::vector<bool> in_closure(states.size());
	std::queue<int> q;
	q.emplace(state);
	in_closure[state] = true;
	while (!q.empty())
	{
		auto current = states[q.front()].get();
		q.pop();
		for (const auto& v : current->transitions())
		{
			if (!v.symbol().is_epsilon())
			{
				continue;
			}
			if (!in_closure[v.state()->state_id()])
			{
				in_closure[v.state()->state_id()] = true;
				q.emplace(v.state()->state_id());
			}
		}
	}
	return in_closure;
}

NFA NFA::construct(const std::vector<symbol_type>& symbols, int begin, int end)
{
	NFA current;
	for (std::size_t i = begin; i < end; ++i)
	{
		if (symbols[i].is_union_op())
		{
			current |= construct(symbols, i + 1, end);
			i = end;
		}
		else if (symbols[i].is_open_paren())
		{
			auto close = close_index(symbols, i);
			if (close + 1 < end && symbols[close + 1].is_kleen_star())
			{
				current += *construct(symbols, i + 1, close);
				i = close + 1;
			}
			else
			{
				current += construct(symbols, i + 1, close);
				i = close;
			}
		}
		else if (i + 1 < end && symbols[i + 1].is_kleen_star())
		{
			current += *NFA(symbols[i]);
			++i;
		}
		else
		{
			current += NFA(symbols[i]);
		}
	}

	return current;
}

int NFA::close_index(const std::vector<symbol_type> & symbols, int idx) const
{
	if (_close_index.empty())
	{
		_close_index.resize(symbols.size(), -1);

		std::vector<int> stack;
		for (std::size_t i = 0; i < symbols.size(); ++i)
		{
			if (symbols[i].is_open_paren())
			{
				stack.emplace_back(i);
			}
			else if (symbols[i].is_close_paren())
			{
				_close_index[stack.back()] = i;
				stack.pop_back();
			}
		}
	}

	return _close_index[idx];
}
