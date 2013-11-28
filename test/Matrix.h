#pragma once

#include <array>
#include <type_traits>
#include <initializer_list>

template <unsigned Row, unsigned Col, class Element=float>
class Matrix{
	std::array<std::array<Element, Col>, Row> mat;

public:
	Matrix(): mat()
	{
	}
	template <class Element2>
	Matrix(const std::initializer_list<Element2> &list): mat()
	{
		auto it = list.begin();

		for(unsigned i = 0; i < Row; ++i){
			for(unsigned j = 0; j < Col; ++j){
				if(it == list.end())
					return;
				mat[i][j] = static_cast<Element>(*it++)	;
			}
		}
	}
	template <class Element2, class... Elements>
	Matrix(Element2 e, Elements... args): Matrix({e, args...})
	{
	}

	std::array<Element, Col> &operator[](int i)
	{
		return mat[i];
	}
	const std::array<Element, Col> &operator[](int i) const
	{
		return mat[i];
	}

	template <unsigned Row2, unsigned Col2, class Element2=float>
	Matrix<Row, Col2, typename std::common_type<Element, Element2>::type> operator*(const Matrix<Row2, Col2, Element2> &m2)
	{
		static_assert(Col == Row2, "dimension mismatch");

		using element_type = std::common_type_t<Element, Element2>;

		Matrix<Row, Col2, element_type> m;

		for(unsigned i = 0; i < Row; ++i){
			for(unsigned k = 0; k < Col; ++k){
				for(unsigned j = 0; j < Col2; ++j){
					m[i][j] += mat[i][k] * m2[k][j];
				}
			}
		}

 		return m;
	}

	template <class Element2=float>
	Matrix<Row, Col, typename std::common_type<Element, Element2>::type> operator+(const Matrix<Row, Col, Element2> &m2)
	{
		using element_type = std::common_type_t<Element, Element2>;
		Matrix<Row, Col, element_type> m;
		for(unsigned i = 0; i < Row; ++i){
			for(unsigned j = 0; j < Col; ++j){
				m[i][j] = (*this)[i][j] + m2[i][j];
			}
		}
		return m;
	}

	template <class Element2=float>
	Matrix<Row, Col, typename std::common_type<Element, Element2>::type> operator-(const Matrix<Row, Col, Element2> &m2)
	{
		using element_type = std::common_type_t<Element, Element2>;
		Matrix<Row, Col, element_type> m;
		for(unsigned i = 0; i < Row; ++i){
			for(unsigned j = 0; j < Col; ++j){
				m[i][j] = (*this)[i][j] - m2[i][j];
			}
		}
		return m;
	}
};
