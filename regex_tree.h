#pragma once

#include <vector>
#include <memory>
#include <unordered_set>

#include "regex.h"

class RegexTree
{
public:
	using symbol_type = Regex::Symbol;


	RegexTree() { }

	RegexTree(const Regex& regex);

protected:

	class Node
	{
	public:
		enum class Type
		{
			CONCAT, UNION, STAR, LEAF
		};


		Node(Type type,
		     std::unique_ptr<Node> left,
		     std::unique_ptr<Node> right = nullptr);

		Node(Type type, symbol_type label);


		bool is_concat() const
		{ return _type == Type::CONCAT; }

		bool is_union() const
		{ return _type == Type::UNION; }

		bool is_star() const
		{ return _type == Type::STAR; }

		bool is_leaf() const
		{ return _type == Type::LEAF; }

		virtual Node* left() const;

		virtual Node* right() const;

		virtual Node* child() const;

		symbol_type label() const;

	protected:
		Type _type;

		symbol_type _label;

		std::vector<std::unique_ptr<Node>> _children;
	};

	template<typename T>
	std::unique_ptr<T> init(const std::vector<symbol_type>& symbols,
	                        std::size_t begin,
	                        std::size_t end);

	void calc_close_index(const std::vector<symbol_type>& symbols);


	std::vector<std::size_t> close_index;

	std::unique_ptr<Node> root;
	std::vector<Node*> _leaves;
};

class AugmentedRegexTree : public RegexTree
{
public:
	using leaf_pos_type = std::size_t;
	using leaves_set_type = std::unordered_set<leaf_pos_type>;


	AugmentedRegexTree(const AugmentedRegex& regex);


	leaves_set_type firstpos_root() const
	{ return static_cast<Node*>(root.get())->firstpos; }

	leaves_set_type followpos(leaf_pos_type leaf_pos) const
	{ return static_cast<Node*>(_leaves[leaf_pos])->followpos; }

	symbol_type label(leaf_pos_type leaf_pos) const
	{ return static_cast<Node*>(_leaves[leaf_pos])->label(); }

protected:
	class Node : public RegexTree::Node
	{
	public:
		using RegexTree::Node::Node;


		Node* left() const override
		{ return static_cast<Node*>(RegexTree::Node::left()); }

		Node* right() const override
		{ return static_cast<Node*>(RegexTree::Node::right()); }

		Node* child() const override
		{ return static_cast<Node*>(RegexTree::Node::child()); }

		leaf_pos_type leaf_pos() const;


		bool nullable;

		leaves_set_type firstpos;
		leaves_set_type lastpos;
		leaves_set_type followpos;

	private:
		friend AugmentedRegexTree::AugmentedRegexTree(
			const AugmentedRegex& regex);


		leaf_pos_type _leaf_pos;
	};


	void calc_nullable(Node* node);

	void calc_first_last_pos(Node* node);

	void calc_followpos(Node* node);
};

template<typename T>
std::unique_ptr<T> RegexTree::init(
	const std::vector<symbol_type>& symbols,
	std::size_t begin,
	std::size_t end)
{
	if (begin >= end)
	{
		// TODO: report error: invalid regular expression
		throw std::exception();
	}

	// .
	if (begin + 1 == end)
	{
		auto leaf =  std::make_unique<T>(T::Type::LEAF,
		                                 symbols[begin]);
		_leaves.emplace_back(leaf.get());
		return leaf;
	}

	if (symbols[begin].is_open_paren())
	{
		auto close = close_index[begin];

		// (...)*...
		if (close + 1 < end && symbols[close + 1].is_kleen_star())
		{
			// (...)*...
			if (close + 2 < end)
			{
				return std::make_unique<T>(T::Type::CONCAT,
				                           init<T>(symbols, begin, close + 2),
				                           init<T>(symbols, close + 2, end));
			}
			// (...)*
			else
			{
				return std::make_unique<T>(T::Type::STAR,
				                           init<T>(symbols, begin + 1, close));
			}
		}
		// (...)...
		else
		{
			// (...)...
			if (close + 1 < end)
			{
				return std::make_unique<T>(T::Type::CONCAT,
				                           init<T>(symbols, begin + 1, close),
				                           init<T>(symbols, close + 1, end));
			}
			// (...)
			else
			{
				return init<T>(symbols, begin + 1, close);
			}
		}
	}

	// ...|...
	int depth = 0;
	for (std::size_t i = begin; i < end; ++i)
	{
		depth += symbols[i].is_open_paren();
		depth -= symbols[i].is_close_paren();
		if (symbols[i].is_union_op() && depth == 0)
		{
			return std::make_unique<T>(T::Type::UNION,
			                           init<T>(symbols, begin, i),
			                           init<T>(symbols, i + 1, end));
		}
	}

	// .*...
	if (begin + 1 < end && symbols[begin + 1].is_kleen_star())
	{
		// .*...
		if (begin + 2 < end)
		{
			return std::make_unique<T>(T::Type::CONCAT,
			                           init<T>(symbols, begin, begin + 2),
			                           init<T>(symbols, begin + 2, end));
		}
		// .*
		else
		{
			return std::make_unique<T>(T::Type::STAR,
			                           init<T>(symbols, begin, begin + 1));
		}
	}

	// ...
	return std::make_unique<T>(T::Type::CONCAT,
	                           init<T>(symbols, begin, begin + 1),
	                           init<T>(symbols, begin + 1, end));
}
