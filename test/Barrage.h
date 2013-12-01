#pragma once

#include "Aliases.h"

#include "Character.h"

#include <chrono>
#include <string>

namespace Barrage{

	class Bullet: public Character{
		bool active = false;

	public:
		void SetActive(bool a=true)
		{
			active = a;
		}
		bool IsActive() const
		{
			return active;
		}
		operator bool() const
		{
			return IsActive();
		}
	};

	class Player: public Character{
	public:
		class Bullet: public ::Barrage::Bullet{

			void Move()
			{
			}
		};
	};


	template <class Window>
	class Scene: public Aliases::Scene<Window>{
		using Base = Aliases::Scene<Window>;

	public:
		Scene(Window *w): Base(w)
		{
		}
	};

}
