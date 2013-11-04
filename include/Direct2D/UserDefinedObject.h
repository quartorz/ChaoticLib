#pragma once

#include "BaseTypes.h"
#include "Resource.h"
#include "Object.h"

#include <unordered_set>
#include <deque>
#include <algorithm>
#include <functional>

#ifdef CreateFont
#undef CreateFont
#endif

namespace ChaoticLib{ namespace Direct2D{

	template <class Derived, class Window, bool CheckDuplicate=false, bool MultiThread=false /* –¢ŽÀ‘• */>
	class UserDefinedObject: public Object{
		Window *window;
		std::unordered_set<Resource*> resources;
		std::deque<Object*> objects;
		Object *hover, *focus;
		bool pushing;
		bool created;

		std::deque<Object*>::reverse_iterator FindObject(const Point &ap)
		{
			return std::find_if(
				objects.rbegin(), objects.rend(),
				std::bind(
					&Object::IsColliding,
					std::placeholders::_1,
					std::cref(ap - this->GetPosition())));
		}

	public:
		UserDefinedObject(Window *w): window(w), hover(nullptr), focus(nullptr), pushing(false), created(false)
		{
		}

		Window *GetWindow()
		{
			return window;
		}
		Window *GetWindow() const
		{
			return const_cast<UserDefinedObject<Derived, Window, CheckDuplicate, MultiThread>*>(this)->GetWindow();
		}

		virtual bool IsColliding(const Point &ap) override
		{
			if(!ap.IsInside(this->GetRect()))
				return false;
			return FindObject(ap) != objects.rend();
		}
		virtual void OnLeftPress(const Point &ap, Object::HitTestStruct &hts) override
		{
			pushing = true;

			auto it = FindObject(ap);
			if(it != objects.rend()){
				focus = *it;
				focus->OnLeftPress(ap - this->GetPosition(), hts);
			}
		}
		virtual void OnMouseMove(const Point &ap, Object::HitTestStruct &hts) override
		{
			if(pushing){
				if(focus != nullptr){
					focus->OnMouseMove(ap - this->GetPosition(), hts);
				}
			}else{
				auto it = FindObject(ap);
				if(it != objects.rend()){
					(*it)->OnMouseMove(ap - this->GetPosition(), hts);
					if(hover != nullptr && hover != *it){
						hover->OnMouseLeave(hts);
					}
					hover = *it;
				}else if(hover != nullptr){
					hover->OnMouseLeave(hts);
					hover = nullptr;
				}
			}
		}
		virtual void OnLeftRelease(const Point &ap, Object::HitTestStruct &hts) override
		{
			pushing = false;
			if(focus != nullptr){
				focus->OnLeftRelease(ap - this->GetPosition(), hts);
			}
		}
		virtual void OnMouseLeave(const Object::HitTestStruct &hts) override
		{
			if(!pushing && hover != nullptr){
				hover->OnMouseLeave(hts);
				hover = nullptr;
			}
		}
		virtual void OnLoseFocus(const Object::HitTestStruct &hts) override
		{
			if(focus != nullptr){
				focus->OnLoseFocus(hts);
				focus = nullptr;
			}
		}

		virtual bool CreateResource(const CreateStruct &cs) override
		{
			created = true;
			return std::all_of(
				resources.begin(), resources.end(),
				std::bind(
					&Resource::CreateResource,
					std::placeholders::_1,
					std::ref(cs)))
				&& std::all_of(
				objects.begin(), objects.end(),
				std::bind(
					&Object::CreateResource,
					std::placeholders::_1,
					std::ref(cs)));
		}

		virtual void DestroyResource() override
		{
			created = false;
			std::for_each(resources.begin(), resources.end(), std::mem_fn(&Resource::DestroyResource));
			std::for_each(objects.begin(), objects.end(), std::mem_fn(&Object::DestroyResource));
		}

		virtual void Draw(const PaintStruct &ps) override
		{
			auto p = this->GetPosition();
			D2D1::Matrix3x2F transform;
			ps.target->GetTransform(&transform);
			ps.target->SetTransform(transform * D2D1::Matrix3x2F::Translation(p.x, p.y));

			ps.target->PushAxisAlignedClip(
				Rect(Point(), this->GetSize()),
				D2D1_ANTIALIAS_MODE_ALIASED);

			std::for_each(
				objects.begin(), objects.end(),
				std::bind(
					&Object::Draw,
					std::placeholders::_1,
					std::ref(ps)));

			ps.target->PopAxisAlignedClip();

			ps.target->SetTransform(transform);
		}

		void RegisterResource(Resource *r)
		{
			if(r == nullptr)
				return;
			if(CheckDuplicate && resources.find(r) != resources.end()){
				// throw std::runtime_error("resource is already registered");
				return;
			}

			if(created)
				r->CreateResource(window->CreateStruct());
			resources.insert(r);
		}

		void UnregisterResource(Resource *r)
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

		void DeleteResource(Resource *r)
		{
			if(r == nullptr)
				return;

			UnregisterResource(r);
			r->DestroyResource();
			delete r;
		}

		void RegisterObject(Object *o)
		{
			if(o == nullptr)
				return;
			if(CheckDuplicate && std::find(objects.begin(), objects.end(), o) != objects.end())
				return;

			if(created)
				o->CreateResource(window->CreateStruct());
			objects.push_back(o);
		}

		void UnregisterObject(Object *o)
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

		void DeleteObject(Object *o)
		{
			if(o == nullptr)
				return;

			UnregisterObject(o);
			o->DestroyResource();
			delete o;
		}

		SolidBrush *CreateSolidBrush(const Color &c)
		{
			auto br = new SolidBrush(c);

			RegisterResource(br);

			return br;
		}

		Font *CreateFont()
		{
			auto font = new Font();

			RegisterResource(font);

			return font;
		}

		Image *CreateImage()
		{
			auto image = new Image;

			RegisterObject(image);

			return image;
		}

		Text *CreateText()
		{
			auto text = new Text;

			RegisterObject(text);

			return text;
		}
	};

} }
