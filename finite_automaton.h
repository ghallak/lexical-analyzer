#pragma once

#include <vector>
#include <utility>
#include <memory>

/**
 * A class for finite automaton.
 */
class FiniteAutomaton
{
protected:
	/**
	 * A class that represents a state of a FiniteAutomaton.
	 */
	struct State
	{
		void add_transition(State* state, char c = '\0')
		{ transitions.push_back(std::make_pair(c, state)); }

		int state_number; /**< unique number for each states */
		std::vector<std::pair<char, State*>> transitions; /**< all transitions from a state */
	};

public:
	using eps_closure_type = std::vector<bool>;

	/**
	 * Check if the FiniteAutomaton is empty.
	 * @return true when the FiniteAutomaton is empty, and false otherwise.
	 */
	bool empty() const
	{ return states_count() == 0; }

	/**
	 * The number of states in the automaton.
	 * @return the number of states
	 */
	int states_count() const
	{ return states.size(); }

	/**
	 * Getter for the alphabet of the FiniteAutomaton.
	 * @return the alphabet of the FiniteAutomaton
	 */
	std::vector<char> alphabet() const;

	/**
	 * Transition from a state to another though a character
	 * @param state_number the number of the states to look from
	 * @param c the character of the transition
	 * @return the number of the founded state and -1 if no state is found
	 */
	int transition(int state_number, char c) const;

	/**
	 * Print the FiniteAutomaton.
	 */
	void print() const;

protected:
	std::vector<std::unique_ptr<State>> states; /**< The states of the FiniteAutomaton */
	mutable std::vector<char> _alphabet;
};
