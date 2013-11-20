#pragma once

#include "FlatButton.h"

#include <vector>

namespace ChaoticLib{

	template <class Traits>
	class DropDownList: public Traits::Object{
		FlatButton<Traits> primary;
	};

}
