#pragma once

#include <vector>
#include <memory>
#include <string>
#include <utility>

class NFA
{
private:
	struct State
	{
		// TODO: variabl state_number is for debugging purpose
		// and should be removed later
		int state_number;
		std::vector<std::pair<char, State*>> adjacent;
	};

public:
	NFA() : head(nullptr), tail(nullptr)
	{ }

	explicit NFA(const std::string& s);

	explicit NFA(char c);

	bool empty()
	{ return head == tail && head == nullptr; }

	NFA& operator+=(NFA&& rhs);

	NFA& operator|=(NFA&& rhs);

	NFA&& operator*() &&;

	void eps_closure(int state);

	void print();

private:
	NFA construct(const std::string& s, int begin, int end);

	int find_close(const std::string& s, int idx);

private:
	State *head;
	State *tail;
	std::vector<std::unique_ptr<State>> states;
};
