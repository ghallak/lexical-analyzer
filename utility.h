namespace utility
{

template<typename T>
T union_sets(const T& s1, const T& s2)
{
	T result = s1;

	result.insert(s2.cbegin(), s2.cend());

	return result;
}

}
