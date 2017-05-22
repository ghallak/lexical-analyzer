#include "regex.h"

Symbol::Symbol(const std::string& symbol)
{
	switch (symbol.length())
	{
		case 0:
			_type = Type::EPS;
			break;

		case 1:
			switch (symbol[0])
			{
				case '|':
					_type = Type::UNION_OP;
					break;
				case '*':
					_type = Type::KLEEN_STAR;
					break;
				case '(':
					_type = Type::OPEN_PAREN;
					break;
				case ')':
					_type = Type::CLOSE_PAREN;
					break;
				default:
					_symbol = symbol;
					_type = Type::CHAR;
			}
			break;

		case 3:
			if (symbol[1] != '-') throw std::exception();

			_symbol = symbol;
			_type = Type::RANGE;
			break;

		default:
			// TODO: change exception
			throw std::exception();
	}
}

std::string Symbol::to_string() const
{
	switch (_type)
	{
		case Type::EPS:         return "EPSILON";
		case Type::OPEN_PAREN:  return "(";
		case Type::CLOSE_PAREN: return ")";
		case Type::KLEEN_STAR:  return "*";
		case Type::UNION_OP:    return "|";
		default:                return _symbol;
	}
}

std::unique_ptr<RegexTree::Node> RegexTree::NodeFactory::create(
	RegexTree::NodeType type,
	std::unique_ptr<RegexTree::Node> left,
	std::unique_ptr<RegexTree::Node> right)
{
	switch (type)
	{
		case NodeType::CONCAT:
			return std::make_unique<Concat>(std::move(left), std::move(right));
		case NodeType::UNION:
			return std::make_unique<Union>(std::move(left), std::move(right));
		case NodeType::STAR:
			return std::make_unique<Star>(std::move(left));
		default:
			// TODO: throw another exception
			throw std::exception();
	}
}

std::unique_ptr<RegexTree::Node> RegexTree::NodeFactory::create(
	RegexTree::NodeType type,
	Symbol symbol)
{
	if (type == NodeType::LEAF)
		return std::make_unique<Leaf>(symbol);
	else
		// TODO: throw another exception
		throw std::exception();
}

RegexTree::RegexTree(const std::string& regex)
{
	std::vector<Symbol> symbols;
	for (std::size_t i = 0; i < regex.length();)
	{
		if (i + 2 < regex.length() && regex[i + 1] == '-')
		{
			symbols.emplace_back(regex.substr(i, 3));
			i += 3;
		}
		else
		{
			symbols.emplace_back(regex[i]);
			++i;
		}
	}

	// Calculate the closing parens indices for each open paren
	calc_close_index(symbols);

	// Set the root for the RegexTree
	root = init(symbols, 0, symbols.size());
}

std::unique_ptr<RegexTree::Node> RegexTree::init(
	const std::vector<Symbol>& symbols,
	std::size_t begin,
	std::size_t end)
{
	if (begin >= end)
	{
		// TODO: report error
		throw std::exception();
	}

	if (begin + 1 == end)
		return NodeFactory::create(NodeType::LEAF, symbols[begin]);

	if (symbols[begin].is_open_paren())
	{
		auto close = close_index[begin];

		// (...)*...
		if (close + 1 < end && symbols[close + 1].is_kleen_star())
		{
			// (...)*...
			if (close + 2 < end)
			{
				return NodeFactory::create(
					NodeType::CONCAT,
					init(symbols, begin, close + 2),
					init(symbols, close + 2, end));
			}
			// (...)*
			else
			{
				return NodeFactory::create(NodeType::STAR,
				                           init(symbols, begin + 1, close));
			}
		}
		// (...)...
		else
		{
			// (...)...
			if (close + 1 < end)
			{
				return NodeFactory::create(
					NodeType::CONCAT,
					init(symbols, begin + 1, close),
					init(symbols, close + 1, end));
			}
			// (...)
			else
			{
				return init(symbols, begin + 1, close);
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
			return NodeFactory::create(
				NodeType::UNION,
				init(symbols, begin, i),
				init(symbols, i + 1, end));
		}
	}

	// .*...
	if (begin + 1 < end && symbols[begin + 1].is_kleen_star())
	{
		// .*...
		if (begin + 2 < end)
		{
			return NodeFactory::create(
				NodeType::CONCAT,
				init(symbols, begin, begin + 2),
				init(symbols, begin + 2, end));
		}
		// .*
		else
		{
			return NodeFactory::create(NodeType::STAR,
			                           init(symbols, begin, begin + 1));
		}
	}

	// ...
	return NodeFactory::create(
		NodeType::CONCAT,
		init(symbols, begin, begin + 1),
		init(symbols, begin + 1, end));
}

void RegexTree::calc_close_index(const std::vector<Symbol>& symbols)
{
	close_index.resize(symbols.size(), symbols.size());

	std::vector<int> stack;
	for (std::size_t i = 0; i < symbols.size(); ++i)
	{
		if (symbols[i].is_open_paren())
		{
			stack.emplace_back(i);
		}
		else if (symbols[i].is_close_paren())
		{
			close_index[stack.back()] = i;
			stack.pop_back();
		}
	}
}
