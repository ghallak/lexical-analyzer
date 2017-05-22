#pragma once

#include <string>
#include <vector>
#include <memory>

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
	RegexTree(const std::string& regex);

private:
	enum class NodeType
	{
		CONCAT, UNION, STAR, LEAF, NODE
	};


	class Node
	{
	public:
		bool is_concat() const
		{ return type() == NodeType::CONCAT; }

		bool is_union() const
		{ return type() == NodeType::UNION; }

		bool is_star() const
		{ return type() == NodeType::STAR; }

		bool is_symbol() const
		{ return type() == NodeType::LEAF; }

	protected:
		virtual NodeType type() const
		{ return NodeType::NODE; }
	};

	class NodeFactory
	{
	public:
		static std::unique_ptr<Node> create(
			NodeType type,
			std::unique_ptr<Node> left,
			std::unique_ptr<Node> right = nullptr);

		static std::unique_ptr<Node> create(NodeType type, Symbol symbol);
	};

	class Concat : public Node
	{
	public:
		Concat(std::unique_ptr<Node> left, std::unique_ptr<Node> right)
			: _right(std::move(right)), _left(std::move(left))
		{ }

		// const Node* left() const
		// { return _left.get(); }
		// const Node* right() const
		// { return _right.get(); }

	protected:
		NodeType type() const override
		{ return NodeType::CONCAT; }

	private:
		std::unique_ptr<Node> _right;
		std::unique_ptr<Node> _left;
	};

	class Union : public Node
	{
	public:
		Union(std::unique_ptr<Node> left, std::unique_ptr<Node> right)
			: _right(std::move(right)), _left(std::move(left))
		{ }

		// const Node* left() const
		// { return _left.get(); }
		// const Node* right() const
		// { return _right.get(); }

	protected:
		NodeType type() const override
		{ return NodeType::UNION; }

	private:
		std::unique_ptr<Node> _right;
		std::unique_ptr<Node> _left;
	};

	class Star : public Node
	{
	public:
		Star(std::unique_ptr<Node> child) : _child(std::move(child))
		{ }

		// const Node* child() const
		// { return _child.get(); }

	protected:
		NodeType type() const override
		{ return NodeType::STAR; }

	private:
		std::unique_ptr<Node> _child;
	};

	class Leaf : public Node
	{
	public:
		Leaf(Symbol symbol) : _symbol(symbol)
		{ }

		// Symbol symbol() const
		// { return _symbol; }

	protected:
		NodeType type() const override
		{ return NodeType::LEAF; }

	private:
		Symbol _symbol;
	};


	std::unique_ptr<Node> init(const std::vector<Symbol>& symbols,
	                           std::size_t begin,
	                           std::size_t end);

	void calc_close_index(const std::vector<Symbol>& symbols);


	std::unique_ptr<Node> root;
	std::vector<std::size_t> close_index;
};
