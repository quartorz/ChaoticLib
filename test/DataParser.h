#pragma once

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <exception>
#include <iterator>
#include <algorithm>

template <class Char, bool ThrowIfError = false>
// Char: charかwchar_t
// ThrowIfError: trueにすると構文エラーが見つかった時に例外を投げる
//
// DataParser<wchar_t, true> dp;
// dp.parse(L"[\"string1\", {\"key0\": [\"string2\"]}]");
// MessageBoxW(nullptr, dp.data().vector()[0]->string().c_str(), dp.data().vector()[1]->map()[L"key0"]->vector()[0]->string().c_str(), MB_OK);
//
// DataParser<wchar_t> dp2;
// dp2.parse(std::wifstream(L"text.txt"));
// MessageBoxW(nullptr, dp2.data().vector()[0]->string().c_str(), dp2.data().vector()[1]->map()[L"key0"]->vector()[0]->string().c_str(), MB_OK);
class DataParser{
	typedef std::basic_string<Char> string_type;

	template <bool Bool = false>
	class OnError{
	public:
		void operator()(...)
		{
		}
	};
	template <>
	class OnError<true>{
	public:
		void operator()(const char *message)
		{
			throw std::runtime_error(message);
		}
	};
	void Error(const char *message)
	{
		OnError<ThrowIfError>()(message);
	}

public:
	class Data{
		friend DataParser<Char, ThrowIfError>;

		typedef std::unordered_map<string_type, Data*> map_type;
		typedef std::vector<Data*> vector_type;

		enum Type{
			None,
			String,
			Map,
			Vector
		}tag;

		union{
			string_type *pstring;
			map_type *pmap;
			vector_type *pvector;
		};

	public:
		Data(): tag(None)
		{
		}
		~Data()
		{
			switch(tag){
			case None:
				return;
			case String:
				delete pstring;
				break;
			case Map:
				std::for_each(pmap->begin(), pmap->end(), [](map_type::value_type &p){delete p.second;});
				delete pmap;
				break;
			case Vector:
				std::for_each(pvector->begin(), pvector->end(), [](Data *p){delete p;});
				delete pvector;
				break;
			default:
				__assume(0);
			}
			tag = None;
		}
		string_type string()
		{
			assert(tag == String);
			return *pstring;
		}
		const string_type &string() const
		{
			return const_cast<Data*>(this)->string();
		}
		map_type &map()
		{
			assert(tag == Map);
			return *pmap;
		}
		const map_type &map() const
		{
			return const_cast<Data*>(this)->map();
		}
		vector_type &vector()
		{
			assert(tag == Vector);
			return *pvector;
		}
		const vector_type &vector() const
		{
			return const_cast<Data*>(this)->vector();
		}
	};

	void parse(const string_type &str)
	{
		parse(data_, str.begin(), str.end());
	}

	void parse(std::basic_istream<Char> &is)
	{
		parse(data_, std::istreambuf_iterator<Char>(is.rdbuf()), std::istreambuf_iterator<Char>());
	}

	Data &data()
	{
		return data_;
	}
	const Data &data() const
	{
		return const_cast<DataParser*>(this)->data();
	}

private:
	Data data_;

	template <class Iter>
	void skip_ws(Iter &it)
	{
		while(*it == static_cast<Char>(' ')
				|| *it == static_cast<Char>('\n')
				|| *it == static_cast<Char>('\r')
				|| *it == static_cast<Char>('\t'))
			it++;
	}

	template <class Iter>
	void skip_comment(Iter &it)
	{
		while(skip_ws(it), *it == static_cast<Char>('#'))
			while(*it != static_cast<Char>('\n') && *it != static_cast<Char>('\r'))
				++it;
	}

	template <class Iter>
	string_type parse_string(Iter &begin, Iter end)
	{
		if(*begin != static_cast<Char>('"'))
			Error("invalid token was found when parsing String (excepted '\"')");

		begin++;

		string_type str;
		auto inserter = std::back_inserter(str);

		while(begin != end){
			if(*begin == static_cast<Char>('"')){
				begin++;
				return str;
			}else if(*begin == static_cast<Char>('\\')){
				begin++;
				if(*begin == static_cast<Char>('n'))
					*inserter = static_cast<Char>('\n');
				else
					*inserter = *begin;
			}else
				*inserter = *begin;
			begin++;
		}

		Error("reached to the end of input (finding '\"')");

		return string_type();
	}

	template <class Iter>
	void parse(Data &data, Iter &begin, Iter end)
	{
		skip_ws(begin);
		skip_comment(begin);

		switch(*begin){
		case static_cast<Char>('"'):
			{
				data.tag = Data::String;
				data.pstring = new string_type(parse_string(begin, end));
			}
			break;
		case static_cast<Char>('{'):
			{
				data.tag = Data::Map;
				data.pmap = new Data::map_type;

				begin++;
				skip_comment(begin);

				string_type str;
				Data d;
				if(*begin != static_cast<Char>('}')){
					for(;;){
						skip_comment(begin);
						str = parse_string(begin, end);
						skip_ws(begin);
						if(*begin != static_cast<Char>(':'))
							Error("invalid token in parsing Map (expected ':')");

						begin++;
						parse(d, begin, end);
						(*data.pmap)[str] = new Data(d);

						skip_comment(begin);
						if(*begin == static_cast<Char>('}'))
							break;
						else if(*begin == static_cast<Char>(',')){
							begin++;
							skip_ws(begin);
						}else
							Error("invalid token was found when parsing Map (expected '}' or ',')");
					}
				}
				begin++;
				d.tag = Data::None;
			}
			break;
		case static_cast<Char>('['):
			{
				data.tag = Data::Vector;
				data.pvector = new Data::vector_type;

				begin++;
				skip_comment(begin);

				Data d;
				if(*begin != static_cast<Char>(']')){
					for(;;){
						parse(d, begin, end);
						skip_ws(begin);

						data.pvector->push_back(new Data(d));

						skip_comment(begin);
						if(*begin == static_cast<Char>(']'))
							break;
						else if(*begin == static_cast<Char>(',')){
							begin++;
							skip_comment(begin);
						}else
							Error("invalid token was found when parsing Vector (expected ']' or ',')");
					}
				}
				begin++;
				d.tag = Data::None;
			}
			break;
		default:
			Error("invalid token");
		}
	}
};
