#pragma once

#include <d2d1.h>
#include "BaseTypes.h"

namespace ChaoticLib{ namespace Direct2D{
	class Resource{
	public:
		virtual ~Resource()
		{
		}

		virtual bool CreateResource(const CreateStruct &) = 0;
		virtual void DestroyResource() = 0;
	};
} }
