#pragma once

#include "ResourceManager.h"
#include "ObjectManager.h"

namespace ChaoticLib{

	template <class Derived, class Traits, bool CheckDuplicate=false, bool MultiThread=false>
	class Manager:
		public ResourceManager<Derived, Traits, CheckDuplicate, MultiThread>,
		public ObjectManager<Derived, Traits, CheckDuplicate, MultiThread>
	{
		typedef ResourceManager<Derived, Traits, CheckDuplicate, MultiThread> ResourceManager;
		typedef ObjectManager<Derived, Traits, CheckDuplicate, MultiThread> ObjectManager;

	public:
		bool CreateResource(const typename Traits::CreateStruct &cs)
		{
			return ResourceManager::CreateResource(cs) && ObjectManager::CreateResource(cs);
		}

		void DestroyResource()
		{
			ResourceManager::DestroyResource();
			ObjectManager::DestroyResource();
		}

		void Draw(const typename Traits::PaintStruct &ps)
		{
			ObjectManager::Draw(ps);
		}
	};

}
