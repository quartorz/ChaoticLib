#pragma once

#include <tuple>

namespace ChaoticLib{ namespace TMP{

	extern void *enabler;

	template <unsigned... I>
	class sequence{
		template <unsigned N>
		class append_impl{
		public:
			using type = sequence<I..., N>;
		};

		template <unsigned...>
		class extract{
		public:
			static const unsigned head = 0;
			using tail = sequence<>;
		};

		template <unsigned Head, unsigned... Tail>
		class extract<Head, Tail...>{
		public:
			static const unsigned head = Head;
			using tail = sequence<Tail...>;
		};

	public:
		template <unsigned N>
		using append = typename append_impl<N>::type;

		static const unsigned head = extract<I...>::head;
		using tail = typename extract<I...>::tail;

		static const unsigned size = sizeof...(I);
	};

	template <unsigned I>
	class make_sequence_impl{
		template <unsigned N, bool = N == 0>
		class maker{
		public:
			using type = sequence<>;
		};
		template <unsigned N>
		class maker<N, false>{
		public:
			using type = typename maker<N - 1>::type::template append<N - 1>;
		};
	public:
		using type = typename maker<I>::type;
	};
	template <unsigned I>
	using make_sequence = typename make_sequence_impl<I>::type;

	template <class... Functions>
	class Caller{
		std::tuple<Functions...> functions;

		template <class Seq, typename std::enable_if<Seq::size != 0>::type*& = enabler, class... Args>
		void CallImpl(Seq, Args... args)
		{
			std::get<Seq::head>(functions)(args...);
			CallImpl(typename Seq::tail(), args...);
		}

		template <class Seq, typename std::enable_if<Seq::size == 0>::type*& = enabler, class... Args>
		void CallImpl(Seq, Args...)
		{
		}

	public:
		Caller(Functions... funcs): functions(funcs...)
		{
		}

		template <class... Args>
		void operator()(Args... args)
		{
			CallImpl(make_sequence<sizeof...(Functions)>(), args...);
		}
	};

	template <class... Functions>
	Caller<Functions...> MakeCaller(Functions... funcs)
	{
		return Caller<Functions...>(funcs...);
	}

	template <class Ret, class Function, class... Args>
	class FunctionWrapper{
		Function function;
		std::tuple<Args...> args;

		template <unsigned... I>
		Ret Call(sequence<I...>)
		{
			return function(std::get<I>(args)...);
		}

	public:
		FunctionWrapper(Function func, Args... args): function(func), args(args...)
		{
		}

		Ret operator()()
		{
			return Call(make_sequence<sizeof...(Args)>);
		}
	};

	template <class Ret, class Function, class... Args>
	FunctionWrapper<Ret, Function, Args...> MakeFunctionWrapper(Function func, Args... args)
	{
		return FunctionWrapper<Ret, Function, Args...>(func, args...);
	}

	class Nil
	{
	};

} }
