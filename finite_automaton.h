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
	 * Give unique id to each state of the FiniteAutomaton states.
	 */
	void set_states_ids();

	/**
	 * A class that represents a state of a FiniteAutomaton.
	 */
	class State
	{
	public:
		using transition_type = std::pair<char, const State *>;

		/**
		 * Returns the id of the state.
		 */
		int state_id() const
		{ return _state_id; }

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

		/**
		 * Updates the transition at a given index.
		 * @param idx the given index
		 * @param state the new state
		 * @param c the new character
		 */
		void update_transition(std::size_t idx, State* state, char c)
		{ _transitions[idx].first = c; _transitions[idx].second = state; }

		friend void FiniteAutomaton::set_states_ids();

	private:
		int _state_id; /**< unique id for each states */
		std::vector<transition_type> _transitions; /**< all transitions from a state */
	};

public:
	using eps_closure_type = std::vector<bool>;

	/**
	 * Check if the FiniteAutomaton is empty.
	 * @return true when the FiniteAutomaton is empty, and false otherwise.
	 */
	bool empty() const
	{ return states.empty(); }

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
	const std::vector<char> & alphabet() const;

	/**
	 * Transition from a state to another though a character
	 * @param state_id the id of the state to look from
	 * @param c the character of the transition
	 * @return the id of the founded state and -1 if no state is found
	 */
	int transition(int state_id, char c) const;

	/**
	 * Print the FiniteAutomaton.
	 */
	void print() const;

protected:
	std::vector<std::unique_ptr<State>> states; /**< The states of the FiniteAutomaton */
	mutable std::vector<char> _alphabet;
};
