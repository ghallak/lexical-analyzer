#include "nfa.h"

#include <queue>
#include <algorithm>

NFA::NFA(char c)
{
	// TODO: SHOULD I EMPLACE OR PUSH BACK ??
	states.emplace_back(std::make_unique<State>());
	states.emplace_back(std::make_unique<State>());

	head = states.front().get();
	tail = states.back().get();

	head->add_transition(tail, c);
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
	q.push(state);
	in_closure[state] = true;
	while (!q.empty())
	{
		auto current = states[q.front()].get();
		q.pop();
		for (const auto& v : current->transitions())
		{
			if (v.symbol() != '\0')
			{
				continue;
			}
			if (!in_closure[v.state()->state_id()])
			{
				in_closure[v.state()->state_id()] = true;
				q.push(v.state()->state_id());
			}
		}
	}
	return in_closure;
}

NFA NFA::construct(const std::string& s, int begin, int end)
{
	NFA current;
	for (int i = begin; i < end; ++i)
	{
		if (s[i] == '|')
		{
			current |= construct(s, i + 1, end);
			i = end;
		}
		else if (s[i] == '(')
		{
			int close = close_index(s, i);
			if (close + 1 < end && s[close + 1] == '*')
			{
				current += *construct(s, i + 1, close);
				i = close + 1;
			}
			else
			{
				current += construct(s, i + 1, close);
				i = close;
			}
		}
		else if (i + 1 < end && s[i + 1] == '*')
		{
			current += *NFA(s[i]);
			++i;
		}
		else
		{
			current += NFA(s[i]);
		}
	}

	return current;
}

int NFA::close_index(const std::string& s, int idx) const
{
	if (_close_index.empty())
	{
		_close_index.resize(s.length(), -1);

		std::vector<int> stack;
		for (std::size_t i = 0; i < s.length(); ++i)
		{
			switch (s[i])
			{
				case '(':
				{
					stack.push_back(i);
					break;
				}
				case ')':
				{
					_close_index[stack.back()] = i;
					stack.pop_back();
					break;
				}
			}
		}
	}
	return _close_index[idx];
}
