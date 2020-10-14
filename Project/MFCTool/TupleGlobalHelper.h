#pragma once
#include <tuple>
#include <fstream>
#include <algorithm>
#include <functional>

namespace TupleGlobalHelper
{
	//auto RefTuple() & noexcept
	//{
	//	return std::make_tuple(
	//		std::ref(MemberVariable),
	//		std::ref(MemberVariable),
	//		std::ref(MemberVariable)
	//		, std::ref(MemberVariable));
	//}

	template<typename..._TupleTypes>
	 std::wostream& operator<<(std::wostream& FOut, const std::tuple<_TupleTypes...>& Tuple)
	{
		 auto TupleStreamOut = [&FOut](const _TupleTypes&... _TupleElems) {
			 auto StreamOut = [&FOut](const auto& _TupleElem) {
				FOut << _TupleElem << std::endl;
			};
			(StreamOut(_TupleElems), ...);
		};
		std::apply(TupleStreamOut, Tuple);

		return FOut;
	}

	template<typename..._TupleTypes>
	 std::wistream& operator>>(std::wistream& FIn, std::tuple<_TupleTypes...>& Tuple)
	{
		 auto TupleStreamIn = [&FIn](_TupleTypes&... _TupleElems) {
			 auto StreamIn = [&FIn](auto& _TupleElem) {
				FIn >> _TupleElem;
			};
			(StreamIn(_TupleElems), ...);
		};
		std::apply(TupleStreamIn, Tuple);

		return FIn;
	}
}

	
	