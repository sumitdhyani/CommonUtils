#pragma once
#include <variant>
#include <string>
#include <regex>
#include <unordered_map>
#include <exception>
namespace ULCommonUtils
{
	//If KeyType is a user defined type, then an operator 
	//void operator +=(std::string& str, KeyType) must be defined
	template<typename KeyType, typename T, typename... Args>
	struct PropertyTree
	{
		typedef std::vector<std::variant<PropertyTree, T, Args...>> Nodes;
		//A node can be a any member of the variant or a list of Nodes
		typedef std::variant<PropertyTree, Nodes, T, Args...> Node;
		typedef std::unordered_map<KeyType, Node> NodeContainer;

		typedef typename NodeContainer::iterator iterator;
		typedef typename NodeContainer::const_iterator const_iterator;

		Node& operator[](const KeyType& attribute)
		{
			return m_data[attribute];
		}

		const Node& operator[](const KeyType& attribute) const throw(const std::runtime_error&)
		{
			auto it = m_data.find(attribute);
			if (it != m_data.end())
				return it->second;
			else
				throw std::runtime_error(std::string("Attribute absent: ") + attribute);
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

		std::pair<iterator, bool> insert(const std::pair<KeyType, Node>& val)
		{
			return m_data.insert(val);
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
		void operator()()
		{
		}
	};
	template<typename Visitor, typename KeyType, typename T, typename... Args>
	std::string serializeToJSon(const PropertyTree<KeyType, T, Args...>& pt);


	namespace
	{
		template<typename VisitorBase, typename Keytype, typename T, typename... Args>
		struct JSonConversionVisitor : VisitorBase
		{
			typedef PropertyTree<Keytype, T, Args...> PropertyTree;
			typedef typename PropertyTree::Nodes Nodes;
			using VisitorBase::operator();


			std::string operator()(int num)
			{
				return std::to_string(num);
			}

			std::string operator()(double num)
			{
				return std::to_string(num);
			}

			std::string operator()(char ch)
			{
				return '\'' + std::string(1, ch) + '\'';
			}

			std::string operator()(std::string str)
			{
				return '\"' + str + '\"';
			}

			std::string operator()(const PropertyTree& pt) const
			{
				return serializeToJSon<VisitorBase, Keytype, T, Args...>(pt);
			}

			std::string operator()(const Nodes& nodes) const
			{
				std::string str;
				str += "[";

				for (auto it = nodes.begin();;)
				{
					str += std::visit(JSonConversionVisitor(), (*it));

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

		PropertyTree<std::string, std::string, char, int, double> deseraliseFromJSon(std::string jsonString, size_t& start);

		inline std::variant< PropertyTree<std::string, std::string, char, int, double>, std::string, char, int, double>  parseValue(std::string jsonString, size_t& start)
		{
			typedef PropertyTree<std::string, std::string, char, int, double> PropertyTree;
			if (jsonString[start] == '\"')
			{
				++start;
				std::string val = jsonString.substr(start, jsonString.find_first_of('\"', start + 1) - start);
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
			else
			{
				auto endNumIndex = jsonString.find_first_not_of("0123456789.", start);
				auto isInteger = (jsonString.find_first_not_of("0123456789", start) == endNumIndex);

				if (isInteger)
				{
					auto retVal = atoi(jsonString.substr(start, endNumIndex - start).c_str());
					start = endNumIndex;
					return retVal;
				}
				else
				{
					auto retVal = atof(jsonString.substr(start, endNumIndex - start).c_str());
					start = endNumIndex;
					return retVal;
				}
			}
		}

		inline std::vector< std::variant< PropertyTree<std::string, std::string, char, int, double>, std::string, char, int, double> > parseValues(std::string jsonString, size_t& start)
		{
			typedef PropertyTree<std::string, std::string, char, int, double> PropertyTree;
			std::vector<std::variant<PropertyTree, std::string, char, int, double>> retVal;
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

		inline PropertyTree<std::string, std::string, char, int, double> deseraliseFromJSon(std::string jsonString, size_t& start)
		{
			typedef PropertyTree<std::string, std::string, char, int, double> PropertyTree;
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
					auto val = std::get<std::string>(parseValue(jsonString, start));
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
					pt[currKey] = std::get<char>(parseValue(jsonString, start));
					currKey = "";
				}
				else if (currentChar == '{')
				{
					pt.insert({ currKey, deseraliseFromJSon(jsonString, start) });
					currKey = "";
				}
				else if (currentChar == '[')
				{
					pt[currKey] = parseValues(jsonString, start);
					currKey = "";
				}
				else if (isdigit(currentChar))
				{
					std::variant<PropertyTree, std::string, char, int, double> val = parseValue(jsonString, start);
					try
					{
						auto integer = std::get<int>(val);
						pt[currKey] = integer;
					}
					catch (std::bad_variant_access& ex)
					{
						auto decimal = std::get<double>(val);
						pt[currKey] = decimal;
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
	std::string serializeToJSon(const PropertyTree<KeyType, T, Args...>& pt)
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
			auto valueForCurrKey = std::visit(JSonConversionVisitor<Visitor, KeyType, T, Args...>(), it->second);
			if ((0 == valueForCurrKey.length()))
				throw std::runtime_error(std::string("Empty value for key: ") + it->first);
			else if ('\"' != valueForCurrKey[0] &&
				'\'' != valueForCurrKey[0] &&
				'[' != valueForCurrKey[0] &&
				'{' != valueForCurrKey[0]
				)//If this is not a string, char, a json string or an array of values, then it must be a valid number
			{
				if (!std::regex_match(valueForCurrKey, std::regex("[-+]?([0-9]+\.[0-9]+|[0-9]+)")))
					throw std::runtime_error(std::string("Incorrectly formatted value provided for key: ") + it->first);
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


	inline PropertyTree<std::string, std::string, char, int, double> deseraliseFromJSon(std::string jsonString) throw (const std::runtime_error&)
	{
		size_t start = 0;
		return deseraliseFromJSon(jsonString, start);
	}

}