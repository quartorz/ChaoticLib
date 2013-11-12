#pragma once

#include <deque>
#include <functional>
#include <algorithm>

namespace ChaoticLib{

	template <class Derived, class Traits, bool CheckDuplicate=false, bool MultiThread=false /* –¢ŽÀ‘• */>
	class ObjectManager{
		std::deque<typename Traits::Object*> objects;
		bool created;

	public:
		ObjectManager(): created(false)
		{
		}
		~ObjectManager()
		{
			std::for_each(objects.begin(), objects.end(), std::mem_fn(&Traits::Object::DestroyResource));
		}

		bool CreateResource(const typename Traits::CreateStruct &cs)
		{
			created = true;
			return std::all_of(
				objects.begin(), objects.end(),
				std::bind(
					&Traits::Object::CreateResource,
					std::placeholders::_1,
					std::ref(cs)));
		}

		void DestroyResource()
		{
			created = false;
			std::for_each(objects.begin(), objects.end(), std::mem_fn(&Traits::Object::DestroyResource));
		}

		void Draw(const typename Traits::PaintStruct &ps)
		{
			std::for_each(
				objects.begin(), objects.end(),
				std::bind(
					&Traits::Object::Draw,
					std::placeholders::_1,
					std::ref(ps)));
		}

		template <class Func>
		void IterateObjects(Func func)
		{
			std::any_of(objects.rbegin(), objects.rend(), [&func](typename Traits::Object *o){return func(*o);});
		}

		void RegisterObject(typename Traits::Object *o)
		{
			if(o == nullptr)
				return;
			if(CheckDuplicate && std::find(objects.begin(), objects.end(), o) != objects.end())
				return;

			if(created)
				o->CreateResource(static_cast<Derived*>(this)->CreateStruct());
			objects.push_back(o);
		}

		void UnregisterObject(typename Traits::Object *o)
		{
			if(o == nullptr)
				return;

			auto it = std::find(objects.begin(), objects.end(), o);
			if(it != objects.end()){
				o->DestroyResource();
				objects.erase(it);
				return;
			}

			// throw std::runtime_error("non-registered object");
		}

		void DeleteObject(typename Traits::Object *o)
		{
			if(o == nullptr)
				return;

			UnregisterObject(o);
			o->DestroyResource();
			delete o;
		}

		// base‚ðnullptr‚É‚·‚é‚Æˆê”Ô‘O‚É—ˆ‚é
		void ReorderObject(typename Traits::Object *move, typename Traits::Object *base, bool behind=false)
		{
			auto it = std::find(objects.begin(), objects.end(), move);
			if(it != objects.end()){
				if(base == nullptr){
					if(it != objects.begin()){
						std::copy_backward(objects.begin(), it - 1, it + 1);
						objects[0] = *it;
					}
				}else{
					auto base_it = std::find(objects.begin(), objects.end(), base);
					if(base_it == objects.end())
						return;
					if(it < base_it){
						std::rotate(it, it + 1, base_it);
						if(behind)
							std::iter_swap(base_it, base_it - 1);
					}else if(it > base_it){
						std::rotate(base_it + 1, it, it + 1);
						if(!behind)
							std::iter_swap(base_it, base_it + 1);
					}
				}
			}
		}

		typename Traits::Image *CreateImage()
		{
			auto image = new Traits::Image;

			RegisterObject(image);

			return image;
		}

		typename Traits::Text *CreateText()
		{
			auto text = new Traits::Text;

			RegisterObject(text);

			return text;
		}
	};

}
