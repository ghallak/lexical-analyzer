#include <iostream>
#include <vector>
#include <memory>
#include <string>

class NFA
{
private:
	struct State
	{
		// TODO: variabl state_number is for debugging purpose
		// and should be removed later
		int state_number;
		std::vector<std::pair<char, State*>> next;
	};

public:
	NFA() : head(nullptr), tail(nullptr)
	{ }

	explicit NFA(char c)
	{
		// TODO: SHOULD I EMPLACE OR PUSH BACK ??
		states.emplace_back(std::make_unique<State>());
		states.emplace_back(std::make_unique<State>());

		head = states.front().get();
		tail = states.back().get();

		head->next.push_back(std::make_pair(c, tail));
	}

	bool empty()
	{
		return head == tail && head == nullptr;
	}

	NFA& operator+=(NFA&& nfa)
	{
		if (empty()) {
			*this = std::move(nfa);
		}
		else {
			tail->next.push_back(std::make_pair('\0', nfa.head));
			tail = nfa.tail;

			states.insert(states.end(),
						make_move_iterator(nfa.states.begin()),
						make_move_iterator(nfa.states.end()));
		}

		return *this;
	}

	NFA& operator|=(NFA&& nfa)
	{
		if (empty()) {
			*this = std::move(nfa);
		}
		else {
			states.emplace_back(std::make_unique<State>());
			auto new_head = states.back().get();
			states.emplace_back(std::make_unique<State>());
			auto new_tail = states.back().get();

			new_head->next.push_back(std::make_pair('\0', head));
			new_head->next.push_back(std::make_pair('\0', nfa.head));

			tail->next.push_back(std::make_pair('\0', new_tail));
			nfa.tail->next.push_back(std::make_pair('\0', new_tail));

			head = new_head;
			tail = new_tail;

			states.insert(states.end(),
						std::make_move_iterator(nfa.states.begin()),
						std::make_move_iterator(nfa.states.end()));
		}
		
		return *this;
	}

	friend NFA operator*(NFA&& nfa);

	void print()
	{
		for (int i = 0; i < (int)states.size(); ++i) {
			auto state = states[i].get();
			state->state_number = i;
		}
		for (int i = 0; i < (int)states.size(); ++i) {
			auto p = states[i].get();
			for (auto s : p->next) {
				std::cout << "from state #" << p->state_number
				          << " through: " << (s.first == '\0' ? 'E' : s.first)
				          << " to state #" << s.second->state_number
				          << '\n';
			}
			std::cout << "--------------\n\n";
		}
	}

private:
	State *head;
	State *tail;
	std::vector<std::unique_ptr<State>> states;
};

NFA operator*(NFA&& nfa)
{
	nfa.states.emplace_back(std::make_unique<NFA::State>());
	auto new_head = nfa.states.back().get();
	nfa.states.emplace_back(std::make_unique<NFA::State>());
	auto new_tail = nfa.states.back().get();

	nfa.tail->next.push_back(std::make_pair('\0', nfa.head));
	nfa.tail->next.push_back(std::make_pair('\0', new_tail));
	new_head->next.push_back(std::make_pair('\0', nfa.head));
	new_head->next.push_back(std::make_pair('\0', new_tail));

	nfa.head = new_head;
	nfa.tail = new_tail;

	return std::move(nfa);
}

int find_close(const std::string& s, int idx)
{
	int open = 1;

	for (int i = idx + 1; i < (int)s.length(); ++i)
	{
		switch (s[i]) {
			case '(': ++open; break;
			case ')': --open; break;
		}

		if (open == 0)
			return i;
	}

	throw std::exception();
}

NFA construct(const std::string& s, int begin, int end)
{
	// TODO: optimize by saving the opening and closing position of each paren

	std::cout << "Constructing: \"" << s.substr(begin, end - begin) << "\"\n";

	NFA current;
	for (int i = begin; i < end; ++i)
	{
		if (s[i] == '|') {
			current |= construct(s, i + 1, end);
			i = end;
		}
		else if (s[i] == '(') {
			int close = find_close(s, i);
			if (close + 1 < end && s[close + 1] == '*') {
				current += *construct(s, i + 1, close);
				i = close + 1;
			}
			else {
				current += construct(s, i + 1, close);
				i = close;
			}
		}
		else if (i + 1 < end && s[i + 1] == '*') {
			current += *NFA(s[i]);
			++i;
		}
		else {
			current += NFA(s[i]);
		}
	}

	return current;
}

int main()
{
	std::string exp = "a(b|c)*";
	auto nfa = construct(exp, 0, exp.length());
	nfa.print();
}
