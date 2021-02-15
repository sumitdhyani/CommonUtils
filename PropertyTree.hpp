#pragma once
#include <variant>
#include <boost/variant.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <regex>
#include <unordered_map>
#include <exception>
namespace ULCommonUtils
{
	template<typename KeyType, typename T, typename... Args>
	struct PropertyTree;

	template<typename KeyType, typename T, typename... Args>
	struct Nodes;

	template<typename KeyType, typename T, typename... Args>
	using ArrayElement = boost::variant<boost::recursive_wrapper<PropertyTree<KeyType, T, Args...>>, boost::recursive_wrapper<Nodes<KeyType, T, Args...>>, T, Args...>;

	template<typename KeyType, typename T, typename... Args>
	struct Nodes
	{
		typedef ULCommonUtils::ArrayElement<KeyType, T, Args...> ArrayElement;
		typedef std::vector<ArrayElement> ArrayElements;

		typedef typename ArrayElements::iterator iterator;
		typedef typename ArrayElements::const_iterator const_iterator;

		typedef typename ArrayElements::reverse_iterator reverse_iterator;
		typedef typename ArrayElements::const_reverse_iterator const_reverse_iterator;

		Nodes() {}

		Nodes(const ArrayElements& elements) : m_elements(elements) {}

		Nodes(size_t n, const ArrayElement& initializer) : m_elements(n, initializer) {}

		Nodes(size_t n, ArrayElement&& initializer) : m_elements(n, initializer) {}

		Nodes(ArrayElements&& elements)
		{
			m_elements.swap(elements);
		}

		iterator begin()
		{
			return m_elements.begin();
		}

		iterator end()
		{
			return m_elements.end();
		}

		const_iterator begin() const
		{
			return m_elements.begin();
		}

		const_iterator end() const
		{
			return m_elements.end();
		}		

		reverse_iterator rbegin()
		{
			return m_elements.rbegin();
		}

		reverse_iterator rend()
		{
			return m_elements.rend();
		}

		const_reverse_iterator rbegin() const
		{
			return m_elements.rbegin();
		}

		const_reverse_iterator rend() const
		{
			return m_elements.rend();
		}

		void push_back(const ArrayElement& elem)
		{
			m_elements.push_back(elem);
		}

		void pop_back()
		{
			m_elements.pop_back();
		}

		size_t size() const
		{
			return m_elements.size();
		}

		bool empty() const
		{
			return m_elements.empty();
		}

		void clear()
		{
			m_elements.clear();
		}

		iterator erase(iterator it)
		{
			return m_elements.erase(it);
		}

		ArrayElement& operator [](size_t index)
		{
			return m_elements[index];
		}

		const ArrayElement& operator [](size_t index) const
		{
			return m_elements[index];
		}

		bool operator ==(const Nodes& other) const
		{
			return (m_elements == other.m_elements);
		}

	private:
		ArrayElements m_elements;

	};

	template<typename KeyType, typename T, typename... Args>
	using Node = boost::variant<boost::recursive_wrapper<PropertyTree<KeyType, T, Args...>>, Nodes<KeyType, T, Args...>, T, Args...>;


	template<typename KeyType, typename T, typename... Args>
	struct PropertyTree
	{
		typedef ULCommonUtils::Nodes<KeyType, T, Args...> Nodes;
		typedef typename Nodes::ArrayElement Node;
		typedef std::unordered_map<KeyType, Node> NodeContainer;
		typedef std::vector<KeyType> Path;

		typedef typename NodeContainer::iterator iterator;
		typedef typename NodeContainer::const_iterator const_iterator;

		Node& operator[](const KeyType& attribute)
		{
			return m_data[attribute];
		}

		const Node& operator[](const KeyType& attribute) const
		{
			auto it = m_data.find(attribute);
			if (it != m_data.end())
				return it->second;
			else
				throw std::runtime_error(std::string("Attribute absent: ") + attribute);
		}

		Node& operator[](const Path& path)
		{
			if (path.empty())
				throw std::runtime_error("Empty path provided");
			else
			{
				PropertyTree* curr = const_cast<PropertyTree*>(this);
				int i;
				for (i = 0; i < path.size() - 1; i++)
				{
					auto it = curr->find(path[i]);
					try
					{
						if (it == curr->end())
							it = curr->insert({path[i], PropertyTree() }).first;

						try
						{
							curr = &boost::get<PropertyTree>(it->second);
							continue;
						}
						catch (boost::bad_get) {}

						auto& nodeList = boost::get<Nodes>(it->second);
						auto listIt = nodeList.begin();
						for(;listIt != nodeList.end(); listIt++)
						{
							try
							{
								curr = &boost::get<PropertyTree>(*listIt);
								break;
							}
							catch(boost::bad_get){ }
						}

						if(listIt == nodeList.end())
							throw boost::bad_get();
					}
					catch (boost::bad_get)
					{
						auto& nodeList = (boost::get<Nodes>(curr->insert_or_assign(path[i], Nodes{{ it->second, PropertyTree() }}).first->second));
						curr = &boost::get<PropertyTree>(nodeList[1]);
					}
				}

				return (*curr)[path[i]];
			}
		}

		const Node& operator[](const Path& path) const
		{
			if(path.empty())
				throw std::runtime_error("Empty path provided");
			else
			{
				const PropertyTree* curr = this;
				int i;
				for (i = 0; i < path.size() - 1; i++)
				{
					try
					{
						if (auto it = (*curr).find(path[i]); it == (*curr).end())
							throw std::runtime_error("Invalid path");
						else
							curr =  &boost::get<PropertyTree>(it->second);
					}
					catch (boost::bad_get)
					{
						throw std::runtime_error("Invalid path");
					}
				}

				return (*curr)[path[i]];
			}
		}

		size_t size() const
		{
			return m_data.size();
		}

		bool empty() const
		{
			return (0 == size());
		}

		iterator begin()
		{
			return m_data.begin();
		}

		iterator end()
		{
			return m_data.end();
		}

		const_iterator begin() const
		{
			return m_data.begin();
		}

		const_iterator end() const
		{
			return m_data.end();
		}

		iterator find(const KeyType& key)
		{
			return m_data.find(key);
		}

		const_iterator find(const KeyType& key) const
		{
			return m_data.find(key);
		}

		iterator erase(const_iterator position)
		{
			return m_data.erase(position);
		}

		size_t erase(const KeyType& key)
		{
			return m_data.erase(key);
		}

		iterator erase(const_iterator first, const_iterator last)
		{
			return m_data.erase(first, last);
		}

		void clear()
		{
			m_data.clear();
		}

		std::pair<iterator, bool> insert(const std::pair<KeyType, Node>& val)
		{
			return m_data.insert(val);
		}

		std::pair<iterator, bool> insert_or_assign(const KeyType& key, const Node& val)
		{
			return m_data.insert_or_assign(key, val);
		}

		PropertyTree() {}

		PropertyTree(const NodeContainer& init)
		{
			m_data = init;
		}

		PropertyTree(NodeContainer&& init)
		{
			m_data.swap(init);
		}

		PropertyTree(const PropertyTree& other)
		{
			m_data = other.m_data;
		}

		PropertyTree(PropertyTree&& other) noexcept
		{
			m_data.swap(other.m_data);
		}

		void swap(PropertyTree& other)
		{
			m_data.swap(other.m_data);
		}

		const PropertyTree& operator=(const PropertyTree& other)
		{
			m_data = other.m_data;
			return *this;
		}

		const PropertyTree& operator=(PropertyTree&& other)
		{
			m_data.swap(other.m_data);
			return *this;
		}

		const PropertyTree& operator=(const NodeContainer& init)
		{
			m_data = init;
			return *this;
		}

		const PropertyTree& operator=(NodeContainer&& init)
		{
			m_data.swap(init);
			return *this;
		}

		bool operator==(const PropertyTree& other) const
		{
			return (m_data == other.m_data);
		}

	private:
		NodeContainer m_data;
	};

	struct NullVisitor
	{
		std::string operator()() const
		{
			return "";
		}
	};
	template<typename Visitor, typename KeyType, typename T, typename... Args>
	std::string serializeToJSon(const PropertyTree<KeyType, T, Args...>& pt);
	
	inline bool validateIsNumber(std::string num)
	{
		if (num.empty())
			return false;

		auto validateIntegerPart = [](std::string num, size_t start)
		{
			size_t end = start;
			if (num[start] == '.')
				return std::string::npos;

			while ((end < num.size()) && (num[end] != '.'))
			{
				if (!isdigit(num[end]))
					return std::string::npos;

				end++;
			}

			return end;
		};


		auto validateZeroOrOneDecimalAndDoesntEndWithDecimal = [](std::string num, size_t start)
		{
			if (start == num.length())//no decimal
				return start;
			else if (num[start] != '.')//Any other character where there should have been a decimal
				return std::string::npos;
			else if (start == num.length() - 1)//ends with decimal
				return std::string::npos;
			else if (num[start + 1] == '.')//2 or more consequetive decimals
				return std::string::npos;
			else
				return start + 1;
		};

		auto validateIntegerToTheEnd = [](std::string num, size_t start)
		{
			size_t end = start;
			while (end < num.length())
			{
				if (!isdigit(num[end]))
					return false;

				end++;
			}

			return true;
		};

		auto res = false;
		auto len = num.length();
		auto start = ((num[0] == '+') || (num[0] == '-'))? 1 : 0;

		auto index = validateIntegerPart(num, start);
		if (index != std::string::npos)
			index = validateZeroOrOneDecimalAndDoesntEndWithDecimal(num, index);

		if(index != std::string::npos)
			res = validateIntegerToTheEnd(num, index);

		return res;
	}

	namespace
	{
		template<typename VisitorBase, typename Keytype, typename T, typename... Args>
		struct JSonConversionVisitor : VisitorBase, boost::static_visitor<std::string>
		{
			typedef ULCommonUtils::PropertyTree<Keytype, T, Args...> PropertyTree;
			typedef typename PropertyTree::Nodes Nodes;
			using VisitorBase::operator();

			std::string operator()(std::string str) const
			{
				return '\"' + str + '\"';
			}

			std::string operator()(char ch) const
			{
				return '\'' + std::string(1, ch) + '\'';
			}

			std::string operator()(short num) const
			{
				return std::to_string(num);
			}

			std::string operator()(int num) const
			{
				return std::to_string(num);
			}

			std::string operator()(long num) const
			{
				return std::to_string(num);
			}

			std::string operator()(long long num) const
			{
				return std::to_string(num);
			}

			std::string operator()(double num) const
			{
				return boost::lexical_cast<std::string>(num);
			}

			std::string operator()(const PropertyTree& pt) const
			{
				return serializeToJSon<VisitorBase, Keytype, T, Args...>(pt);
			}

			std::string operator()(const Nodes& nodes) const
			{
				std::string str;
				str += "[";

				for (auto it = nodes.begin(); it != nodes.end();)
				{
					str += boost::apply_visitor(*this, (*it));

					it++;
					if (it != nodes.end())
						str += ',';
					else
						break;
				}

				str += "]";

				return str;
			}
		};

		typedef ULCommonUtils::PropertyTree<std::string, std::string, char, int, long long, double> PropertyTree;

		PropertyTree deseraliseFromJSon(std::string jsonString, size_t& start);
		PropertyTree::Nodes parseValues(std::string jsonString, size_t& start);

		inline PropertyTree::Node parseValue(std::string jsonString, size_t& start)
		{
			if (jsonString[start] == '\"')
			{
				++start;
				auto endIndex = jsonString.find_first_of('\"', start + 1);
				std::string val(jsonString.c_str() + start, endIndex - start);
				start += val.length() + 1;
				return val;
			}
			else if (jsonString[start] == '\'')
			{
				++start;
				auto val = jsonString[start];
				start += 2;
				return val;
			}
			else if (jsonString[start] == '{')
				return deseraliseFromJSon(jsonString, start);
			else if (jsonString[start] == '[')
				return parseValues(jsonString, start);
			else
			{
				bool isDecimal = false;
				auto endNumIndex = start + 1;
				for (auto currChar = jsonString[endNumIndex]; (0 != std::isdigit(currChar)) || ('.' == currChar); currChar = jsonString[++endNumIndex]) 
				{
					isDecimal = isDecimal || ('.' == jsonString[endNumIndex]);
				}

				std::string numStr(jsonString.c_str() + start,  endNumIndex - start);
				start = endNumIndex;

				if (isDecimal)
					return std::stod(numStr);

				try
				{
					return std::stoi(numStr);
				}
				catch (std::out_of_range)
				{
					return std::stoll(numStr);
				}
			}
		}

		inline PropertyTree::Nodes parseValues(std::string jsonString, size_t& start)
		{
			PropertyTree::Nodes retVal;
			start++;
			while (jsonString[start] != ']')
			{
				while ((jsonString[start] == ',') ||	//Skip the non-data characters
					(jsonString[start] == ' ')
					)
					start++;

				retVal.push_back(parseValue(jsonString, start));

				while (jsonString[start] == ' ')//skip whilte spaces at the end of the list
					start++;
			}
			start++;

			return retVal;
		}

		inline PropertyTree deseraliseFromJSon(std::string jsonString, size_t& start)
		{
			PropertyTree pt;

			std::string currKey = "";

			for (++start; start < jsonString.length();)
			{
				while ((jsonString[start] == ',') ||	//Skip the non-data characters
					(jsonString[start] == ':') ||
					(jsonString[start] == ' ')
					)
					start++;

				char currentChar = jsonString[start];
				if (currentChar == '\"')
				{
					auto val = boost::get<std::string>(parseValue(jsonString, start));
					if (currKey.empty())
						currKey = val;
					else
					{
						pt[currKey] = val;
						currKey = "";
					}
				}
				else if (currentChar == '\'')
				{
					pt[currKey] = boost::get<char>(parseValue(jsonString, start));
					currKey = "";
				}
				else if (currentChar == '{')
				{
					pt[currKey] = deseraliseFromJSon(jsonString, start);
					currKey = "";
				}
				else if (currentChar == '[')
				{
					pt[currKey] = parseValues(jsonString, start);
					currKey = "";
				}
				else if (isdigit(currentChar))
				{
					auto val = parseValue(jsonString, start);
					try
					{
						pt[currKey] = boost::get<int>(val);
					}
					catch (boost::bad_get)
					{
						try
						{
							pt[currKey] = boost::get<double> (val);
						}
						catch (boost::bad_get)
						{
							pt[currKey] = boost::get<long long>(val);
						}
					}

					currKey = "";
				}
				else if (currentChar == '}')
				{
					start++;
					break;
				}
				else
					throw std::runtime_error(std::string("Malformed JSon message, unexpected character \'") + currentChar + "\' at position " + std::to_string(start));
			}

			return pt;
		}

		void operator +=(std::string& str, int val)
		{
			str += std::to_string(val);
		}

		void operator +=(std::string& str, long long val)
		{
			str += std::to_string(val);
		}

		void operator +=(std::string& str, float val)
		{
			str += std::to_string(val);
		}

		void operator +=(std::string& str, double val)
		{
			str += std::to_string(val);
		}

		std::string operator +(std::string str, int val)
		{
			return (str + std::to_string(val));
		}

		std::string operator +(std::string str, long long val)
		{
			return (str + std::to_string(val));
		}

		std::string operator +(std::string str, float val)
		{
			return (str + std::to_string(val));
		}

		std::string operator +(std::string str, double val)
		{
			return (str + std::to_string(val));
		}
	}

	template<typename Visitor, typename KeyType, typename T, typename... Args>
	std::string serializeToJSon(const ULCommonUtils::PropertyTree<KeyType, T, Args...>& pt)
	{
		auto it = pt.begin();
		std::string str;
		str += '{';

		while (it != pt.end())
		{
			str += '\"';
			str += it->first;
			str += '\"';
			str += ':';
			std::string valueForCurrKey = boost::apply_visitor(JSonConversionVisitor<Visitor, KeyType, T, Args...>(), it->second);
			if ((0 == valueForCurrKey.length()))
				throw std::runtime_error(std::string("Empty value for key: ") + it->first);
			else if ('\"' != valueForCurrKey[0] &&
				'\'' != valueForCurrKey[0] &&
				'[' != valueForCurrKey[0] &&
				'{' != valueForCurrKey[0]
				)//If this is not a string, char, a json string or an array of values, then it must be a valid number
			{
				if(!validateIsNumber(valueForCurrKey))
					throw std::runtime_error(std::string("Incorrectly formatted value provided for key: ") + it->first);
				//if (!std::regex_match(valueForCurrKey, std::regex("[-+]?([0-9]+\.[0-9]+|[0-9]+)")))
				//	throw std::runtime_error(std::string("Incorrectly formatted value provided for key: ") + it->first);
			}

			str += valueForCurrKey;

			it++;
			if (it != pt.end())
				str += ',';
			else
				break;
		}


		str += '}';

		return str;
	}


	inline ULCommonUtils::PropertyTree<std::string, std::string, char, int, long long, double> deseraliseFromJSon(std::string jsonString)
	{
		size_t start = 0;
		return deseraliseFromJSon(jsonString, start);
	}

}
