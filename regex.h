#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_set>

class Symbol
{
public:
	Symbol(const std::string& symbol = "");

	Symbol(char c) : Symbol(std::string(1, c))
	{ }


	bool operator==(const Symbol& rhs) const
	{ return _symbol == rhs._symbol && _type == rhs._type; }

	bool operator!=(const Symbol& rhs) const
	{ return !(*this == rhs); }


	bool is_epsilon() const
	{ return _type == Symbol::Type::EPS; }

	bool is_char() const
	{ return _type == Symbol::Type::CHAR; }

	bool is_range() const
	{ return _type == Symbol::Type::RANGE; }

	bool is_kleen_star() const
	{ return _type == Symbol::Type::KLEEN_STAR; }

	bool is_union_op() const
	{ return _type == Symbol::Type::UNION_OP; }

	bool is_open_paren() const
	{ return _type == Symbol::Type::OPEN_PAREN; }

	bool is_close_paren() const
	{ return _type == Symbol::Type::CLOSE_PAREN; }

	std::string to_string() const;

private:
	enum class Type
	{
		EPS,
		CHAR,
		RANGE,
		OPEN_PAREN,
		CLOSE_PAREN,
		KLEEN_STAR,
		UNION_OP
	};


	std::string _symbol;
	Type _type;
};

namespace std
{
	template<>
	struct hash<::Symbol>
	{
		using argument_type = ::Symbol;
		using result_type = size_t;

		result_type operator()(const argument_type& arg) const
		{ return hash<string>()(arg.to_string()); }
	};
}

class RegexTree
{
private:
	class Node;

public:
	using leaf_pos_type = std::size_t;
	using leaves_set_type = std::unordered_set<leaf_pos_type>;


	RegexTree(const std::string& regex);


	std::vector<Symbol> alphabet() const;

	leaves_set_type firstpos_root() const
	{ return root->firstpos; }

	leaves_set_type followpos(leaf_pos_type leaf_pos) const
	{ return _leaves[leaf_pos]->followpos; }

	Symbol label(leaf_pos_type leaf_pos) const
	{ return _leaves[leaf_pos]->label(); }

private:
	enum class NodeType
	{
		CONCAT, UNION, STAR, LEAF
	};


	class Node
	{
	public:
		Node(NodeType type,
		     std::unique_ptr<Node> left,
		     std::unique_ptr<Node> right = nullptr);

		Node(NodeType type, Symbol label, leaf_pos_type pos);


		bool is_concat() const
		{ return _type == NodeType::CONCAT; }

		bool is_union() const
		{ return _type == NodeType::UNION; }

		bool is_star() const
		{ return _type == NodeType::STAR; }

		bool is_leaf() const
		{ return _type == NodeType::LEAF; }

		Node* left() const;

		Node* right() const;

		Node* child() const;

		leaf_pos_type leaf_pos() const;

		Symbol label() const;


		bool nullable;

		leaves_set_type firstpos;
		leaves_set_type lastpos;
		leaves_set_type followpos;

	private:
		NodeType _type;

		Symbol _label;
		leaf_pos_type _leaf_pos;

		std::vector<std::unique_ptr<Node>> _children;
	};


	std::unique_ptr<Node> init(const std::vector<Symbol>& symbols,
	                           std::size_t begin,
	                           std::size_t end);

	void calc_close_index(const std::vector<Symbol>& symbols);

	void calc_nullable(Node* node);

	void calc_first_last_pos(Node* node);

	void calc_followpos(Node* node);


	std::unique_ptr<Node> root;

	std::vector<std::size_t> close_index;
	std::vector<Node*> _leaves;
};
