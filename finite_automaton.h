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
	 * Give unique number to each state of the FiniteAutomaton states.
	 */
	void set_states_numbers() const;

	/**
	 * A class that represents a state of a FiniteAutomaton.
	 */
	class State
	{
	public:
		using transition_type = std::pair<char, const State *>;

		/**
		 * Returns The number of the state.
		 */
		int state_number() const
		{ return _state_number; }

		/**
		 * Returns a const reference to transitions vector.
		 */
		const std::vector<transition_type> & transitions() const
		{ return _transitions; }

		/**
		 * Adds a transition from the state.
		 * @param state the state to add a transition to
		 * @param c the character lable of the transition
		 */
		void add_transition(State* state, char c = '\0')
		{ _transitions.push_back(std::make_pair(c, state)); }

		friend void FiniteAutomaton::set_states_numbers() const;

	private:
		int _state_number; /**< unique number for each states */
		std::vector<transition_type> _transitions; /**< all transitions from a state */
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
