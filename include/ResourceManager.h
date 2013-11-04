#pragma once

#include <unordered_set>
#include <functional>
#include <algorithm>

namespace ChaoticLib{

	template <class Derived, class Traits, bool CheckDuplicate=false, bool MultiThread=false /* –¢ŽÀ‘• */>
	class ResourceManager{
		std::unordered_set<typename Traits::Resource*> resources;
		bool create;

	public:
		ResourceManager():
		  create(false)
		{
		}

		~ResourceManager()
		{
			std::for_each(resources.begin(), resources.end(), std::mem_fn(&Traits::Resource::DestroyResource));
		}

		bool CreateResource(const typename Traits::Resource::CreateStruct &cs)
		{
			create = true;
			return std::all_of(
				resources.begin(), resources.end(),
				std::bind(
					&Traits::Resource::CreateResource,
					std::placeholders::_1,
					std::ref(cs)));
		}

		void DestroyResource()
		{
			create = false;
			std::for_each(resources.begin(), resources.end(), std::mem_fn(&Traits::Resource::DestroyResource));
		}

		void RegisterResource(typename Traits::Resource *r)
		{
			if(r == nullptr)
				return;
			if(CheckDuplicate && resources.find(r) != resources.end()){
				// throw std::runtime_error("resource is already registered");
				return;
			}

			if(create)
				r->CreateResource(static_cast<Derived*>(this)->CreateStruct());
			resources.insert(r);
		}

		void UnregisterResource(typename Traits::Resource *r)
		{
			if(r == nullptr)
				return;

			auto it = resources.find(r);
			if(it != resources.end()){
				r->DestroyResource();
				resources.erase(it);
				return;
			}

			// throw std::runtime_error("non-registered resource");
		}

		void DeleteResource(typename Traits::Resource *r)
		{
			if(r == nullptr)
				return;

			UnregisterResource(r);
			r->DestroyResource();
			delete r;
		}

		typename Traits::SolidBrush *CreateSolidBrush(const typename Traits::Color &c)
		{
			auto br = new Traits::SolidBrush(c);

			RegisterResource(br);

			return br;
		}

		typename Traits::Font *CreateFont()
		{
			auto font = new Traits::Font();

			RegisterResource(font);

			return font;
		}
	};

}
