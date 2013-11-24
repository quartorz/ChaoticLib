#pragma once

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <exception>
#include <iterator>
#include <algorithm>

template <class Char>
class Data{
	typedef std::basic_string<Char> string_type;
	typedef std::unordered_map<string_type, Data> map_type;
	typedef std::vector<Data> vector_type;

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
	Data(const Data&) = delete;
	Data(): tag(None)
	{
	}
	Data(Data &&d): tag(d.tag)
	{
		pstring = d.pstring;
		d.tag = None;
		d.pstring = nullptr;
	}
	~Data()
	{
		clear();
	}
	Data &operator=(Data &&d)
	{
		tag = d.tag;
		pstring = d.pstring;
		d.tag = None;
		d.pstring = nullptr;
		return *this;
	}
	void clear()
	{
		switch(tag){
		case None:
			break;
		case String:
			delete pstring;
			break;
		case Map:
			delete pmap;
			break;
		case Vector:
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
	void set_string(const string_type &string)
	{
		if(tag != String){
			clear();
			tag = String;
			pstring = new string_type(string);
		}
		*pstring = string;
	}
	void make_map()
	{
		clear();
		tag = Map;
		pmap = new map_type;
	}
	void make_vector()
	{
		clear();
		tag = Vector;
		pvector = new vector_type;
	}

	void clear_tag()
	{
		tag = None;
	}

	void save(std::basic_ostream<Char> &os, bool beautify = false)
	{
		save(os, *this, beautify, 0u, true);
	}

	template <bool Throw = false>
	static Data parse(const string_type &s)
	{
		Data data;
		DataParser<Throw>().parse(data, str.begin(), str.end());
		return data;
	}

	template <bool Throw = false>
	static Data parse(std::basic_istream<Char> &is)
	{
		Data data;
		DataParser<Throw>().parse(data, std::istreambuf_iterator<Char>(is.rdbuf()), std::istreambuf_iterator<Char>());
		return data;
	}

private:
	typedef std::basic_ostream<Char> ostream;

	static void print_string(ostream &os, const string_type &s)
	{
		os << static_cast<Char>('"');
		for(auto &c: s){
			switch(c){
			case static_cast<Char>('"') :
				os << static_cast<Char>('\\') << static_cast<Char>('"');
				break;
			case static_cast<Char>('\n') :
				os << static_cast<Char>('\\') << static_cast<Char>('n');
				break;
			case static_cast<Char>('\\') :
				os << static_cast<Char>('\\') << static_cast<Char>('\\');
				break;
			default:
				os << c;
			}
		}
		os << static_cast<Char>('"');
	}

	static void print_indent(ostream &os, unsigned depth)
	{
		for(unsigned i = 0; i < depth; ++i)
			os << static_cast<Char>('\t');
	}

	static void save(ostream &os, Data &d, bool beautify, unsigned depth, bool indent)
	{
		if(!indent && beautify)
			print_indent(os, depth);

		switch(d.tag){
		case String:
			print_string(os, *d.pstring);
			break;
		case Map:
			os << static_cast<Char>('{');
			if(beautify)
				os << static_cast<Char>('\n');
			{
				map_type::size_type count = 0, size = d.pmap->size();
				for(auto &p: *d.pmap){
					if(beautify)
						print_indent(os, depth + 1);
					print_string(os, p.first);
					os << static_cast<Char>(':');
					if(beautify)
						os << static_cast<Char>(' ');
					save(os, p.second, beautify, depth + 1, true);
					if(count != size - 1)
						os << static_cast<Char>(',');
					if(beautify)
						os << static_cast<Char>('\n');
					count++;
				}
			}
			if(beautify)
				print_indent(os, depth);
			os << static_cast<Char>('}');
			break;
		case Vector:
			os << static_cast<Char>('[');
			if(beautify)
				os << static_cast<Char>('\n');
			{
				vector_type::size_type count = 0, size = d.pvector->size();
				for(auto &v: *d.pvector){
					save(os, v, beautify, beautify + 1, false);
					if(count != size - 1)
						os << static_cast<Char>(',');
					if(beautify)
						os << static_cast<Char>('\n');
					count++;
				}
			}
			if(beautify)
				print_indent(os, depth);
			os << static_cast<Char>(']');
			break;
		default:
			__assume(0);
		}
	}

	template <bool Throw>
	class DataParser{
		friend Data<Char>;

		typedef std::basic_string<Char> string_type;
		typedef Data<Char> data_type;

		template <bool Throw = Throw>
		void Error(const char *message)
		{
			throw std::runtime_error(message);
		}

		template <>
		void Error<false>(const char *message)
		{
		}
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
				} else if(*begin == static_cast<Char>('\\')){
					begin++;
					if(*begin == static_cast<Char>('n'))
						*inserter = static_cast<Char>('\n');
					else
						*inserter = *begin;
				} else
					*inserter = *begin;
				begin++;
			}

			Error("reached to the end of input (expected '\"')");

			return string_type();
		}

		template <class Iter>
		void parse(data_type &data, Iter &begin, Iter end)
		{
			skip_ws(begin);
			skip_comment(begin);

			switch(*begin){
			case static_cast<Char>('"') :
				data.set_string(parse_string(begin, end));

				break;
			case static_cast<Char>('{') :
				data.make_map();

				begin++;
				skip_comment(begin);

				if(*begin != static_cast<Char>('}')){
					string_type str;
					data_type d;

					for(;;){
						skip_comment(begin);
						str = parse_string(begin, end);
						skip_ws(begin);
						if(*begin != static_cast<Char>(':'))
							Error("invalid token in Map (expected ':')");

						begin++;
						parse(d, begin, end);
						data.map()[str] =  std::move(d);
						d.clear_tag();

						skip_comment(begin);
						if(*begin == static_cast<Char>('}'))
							break;
						else if(*begin == static_cast<Char>(',')){
							begin++;
							skip_ws(begin);
						} else
							Error("invalid token was found when parsing Map (expected '}' or ',')");
					}
				}
				begin++;

				break;
			case static_cast<Char>('[') :
				data.make_vector();

				begin++;
				skip_comment(begin);

				if(*begin != static_cast<Char>(']')){
					data_type d;

					for(;;){
						data.vector().emplace_back();

						parse(data.vector().back(), begin, end);
						skip_ws(begin);
						d.clear_tag();

						skip_comment(begin);
						if(*begin == static_cast<Char>(']'))
							break;
						else if(*begin == static_cast<Char>(',')){
							begin++;
							skip_comment(begin);
						} else
							Error("invalid token was found when parsing Vector (expected ']' or ',')");
					}
				}
				begin++;

				break;
			default:
				Error("invalid token");
			}
		}
	};
};
