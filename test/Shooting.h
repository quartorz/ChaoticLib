#pragma once

#include "Aliases.h"

#include "KeyConfig.h"
#include "Function.h"
#include "Character.h"
#include "Singleton.h"

#include <chrono>
#include <deque>
#include <random>
#include <ctime>

namespace Shooting{

	class ResourceHolder: public Aliases::Resource, public Singleton<ResourceHolder>{
	public:
		Aliases::SolidBrush black_brush, yellow_brush;
		Aliases::Image player_image, enemy1_image;

		ResourceHolder()
		{
			yellow_brush.SetColor(Aliases::Color(255, 255, 0));
			player_image.SetFileName((GetDirectory() + L"Player.png").c_str());
			player_image.SetSize(Aliases::Size(32, 32));
			enemy1_image.SetFileName((GetDirectory() + L"enemy1.png").c_str());
			enemy1_image.SetSize(Aliases::Size(30, 30));
		}

		bool CreateResource(const Aliases::CreateStruct &cs) override
		{
			if(!black_brush.CreateResource(cs))
				return false;
			if(!yellow_brush.CreateResource(cs))
				return false;
			if(!player_image.CreateResource(cs))
				return false;
			if(!enemy1_image.CreateResource(cs))
				return false;
			return true;
		}
		void DestroyResource() override
		{
			black_brush.DestroyResource();
			yellow_brush.DestroyResource();
			player_image.DestroyResource();
			enemy1_image.DestroyResource();
		}
	};

	class Bullet: public Character{
	public:
		Bullet()
		{
			SetRadius(5.f);
		}
		void Draw(const Aliases::PaintStruct &ps, const Aliases::Brush &brush)
		{
			GetCircle().Fill(ps, brush);
		}
	};

	class Player: public Character{
		ResourceHolder *holder;

	public:
		class Bullet: public ::Shooting::Bullet{
			bool active = false;
			Transform transform = Transform::Identity();

		public:
			void SetTransform(Transform t)
			{
				transform = t;
			}
			void SetPosition(Aliases::Point p)
			{
				Character::SetPosition(p);
				if(!GetPosition().IsInside(Aliases::Rect(-5, -5, 805, 605)))
					SetActive(false);
			}
			void Move()
			{
				SetVector(transform * GetVector());
				Character::Move();
				if(!GetPosition().IsInside(Aliases::Rect(-5, -5, 805, 605)))
					SetActive(false);
			}
			operator bool() const
			{
				return active;
			}
			void SetActive(bool a=true)
			{
				active = a;
			}
		};

		Player(): holder(ResourceHolder::GetInstance())
		{
			this->SetSize(Aliases::Size(32, 32));
			this->SetRadius(4.f);
		}
		~Player()
		{
		}

		void Draw(const Aliases::PaintStruct &ps)
		{
			auto &image = holder->player_image;
			image.SetPosition(this->GetPosition());
			image.Draw(ps);
			GetCircle().Draw(ps, holder->black_brush);
		}
	};

	class Enemy: public Character{
	protected:
		ResourceHolder *holder;

		int hitpoint;
		std::chrono::system_clock::time_point time;

		std::mt19937_64 rand{std::time(nullptr)};

	public:
		class Bullet: public ::Shooting::Bullet{
		protected:
			ResourceHolder *holder;

		public:
			Bullet(): holder(ResourceHolder::GetInstance())
			{
			}
			virtual ~Bullet()
			{
			}
			virtual void Move(const Player &) = 0;
			virtual void Draw(const Aliases::PaintStruct &ps) = 0;
		};
		using bullets_type = std::deque<Bullet*>;

		Enemy(): holder(ResourceHolder::GetInstance())
		{
		}
		virtual ~Enemy()
		{
		}
		void SetHitPoint(int hp)
		{
			hitpoint = hp;
		}
		int GetHitPoint() const
		{
			return hitpoint;
		}

		virtual void Move(const Player &, bullets_type &) = 0;
		virtual void Draw(const Aliases::PaintStruct &) = 0;
	};

	class Enemy1: public Enemy{
		class Bullet: public Enemy::Bullet{
		public:
			void Move(const Player &player) override
			{
				Character::Move();
			}
			void Draw(const Aliases::PaintStruct &ps) override
			{
				GetCircle().Fill(ps, holder->black_brush);
			}
		};

	public:
		Enemy1()
		{
			this->SetRadius(15.f);
			this->SetSize(Aliases::Size(30, 30));
			this->time = std::chrono::system_clock::now();
		}
		~Enemy1()
		{
		}

		void Move(const Player &player, bullets_type &bullets) override
		{
			auto now = std::chrono::system_clock::now();

			auto v = player.GetCenter() - this->GetCenter() + Aliases::Point(10, 10);
			auto rad = std::atan2(v.y, v.x);

			SetVector(Transform::Rotation(rad) * Vector(3.f, 0.f));
			Character::Move();

			if(std::chrono::duration_cast<std::chrono::milliseconds>(now - this->time).count() >= 500){
				this->time = now;
				bullets.push_back(new Bullet);
				auto &b = bullets.back();
				b->SetPosition(GetCenter() - Aliases::Point(GetRadius(), GetRadius()));
				b->SetVector(Transform::Rotation(rad) * Vector(static_cast<float>(rand() % 4 + 3), 0.f));
			}
		}
		void Draw(const Aliases::PaintStruct &ps) override
		{
			auto &image = holder->enemy1_image;
			image.SetPosition(GetPosition());
			image.Draw(ps);
		}
	};

	template <class Window>
	class Scene: public Aliases::Scene<Window>{
		using Base = Aliases::Scene<Window>;

		std::mt19937_64 rand{std::time(nullptr)};

		KeyConfig *config;
		ResourceHolder *holder;

		Player player;

		using bullets_type = std::array<Player::Bullet, 20>;
		bullets_type bullets;
		std::chrono::system_clock::time_point bullet_time;

		std::deque<Enemy*> enemies;
		Enemy::bullets_type enemy_bullets;
		std::chrono::system_clock::time_point enemy_time;
		std::chrono::milliseconds enemy_duration;

		bool pause = false;

	public:
		Scene(Window *w): Base(w), config(KeyConfig::GetInstance()), holder(ResourceHolder::GetInstance())
		{
			this->RegisterResource(holder);

			for(auto &b: bullets){
				b.SetVector(Vector(0, -10));
			}

			this->AddKeyboardHandler([this](unsigned keycode, bool push){
				config->SetKeyboardState(keycode, push);
			}, keycode_range(0, 255));

			this->AddTimerHandler([this](unsigned){
				if(pause)
					return;

				auto state = config->GetState();
				Vector v;
				float velocity = 5.f;

				if(state.up){
					v[1][0] -= velocity;
				}
				if(state.down){
					v[1][0] += velocity;
				}
				if(state.left){
					v[0][0] -= velocity;
				}
				if(state.right){
					v[0][0] += velocity;
				}
				player.SetVector(v);

				auto now = std::chrono::system_clock::now();

				if(state.b && now -  bullet_time >= std::chrono::milliseconds(240)){
					auto it = std::find(bullets.begin(), bullets.end(), false);
					if(it != bullets.end()){
						float r = it->GetRadius();
						it->SetPosition(player.GetCenter());
						it->SetVector(Vector(0, -10));
						it->SetActive();

						auto trans = Transform::Identity();
						if(state.a){
							trans = Transform::Rotation(ToRad(-0.5));
						}
						if(state.c){
							trans = Transform::Rotation(ToRad(0.5)) * trans;
						}
						it->SetTransform(trans);
					}

					bullet_time = now;
				}

				if(now - enemy_time >= enemy_duration){
					enemy_duration = std::chrono::milliseconds(rand() % 750 + 250);
					enemy_time = now;
					enemies.push_back(new Enemy1);
					enemies.back()->SetPosition(Aliases::Point(static_cast<float>(rand() % 800), -50));
				}

				Move();

				this->GetWindow()->Repaint();
			}, 100);
		}
		~Scene()
		{
			this->UnregisterResource(holder);
			ResourceHolder::DeleteInstance();
		}
		void Show() override
		{
			pause = false;
			player.SetPosition(Aliases::Point(384, 500));
		}
		void Hide() override
		{
			for(auto &b: bullets){
				b.SetActive(false);
			}

			for(auto &b: enemy_bullets){
				delete b;
			}
			enemy_bullets.clear();

			for(auto &e: enemies){
				delete e;
			}
			enemies.clear();
		}
		void OnGetJoystickState(const joystick_id &id, const DIJOYSTATE2 &js) override
		{
			config->SetJoystickState(id, js);
		}
		void Gameover()
		{
			pause = true;

			if(::MessageBoxW(this->GetWindow()->GetHwnd(), L"タイトルに戻りますか？", L"ゲームオーバー", MB_YESNO) == IDYES){
				this->GetWindow()->SelectScene(Window::Scene::Title);
			}

			Hide();
			Show();
		}
		void Move()
		{
			for(auto &b: bullets){
				if(b){
					b.Move();
				}
			}

			player.Move();
			auto p = player.GetPosition();
			auto r = Aliases::Rect(p, player.GetSize());
			auto s = this->GetSize();
			if(r.left < 0.f)
				p.x = 0.f;
			else if(r.right >= s.width)
				p.x = s.width - 1.f - r.Width();
			if(r.top < 0.f)
				p.y = 0.f;
			else if(r.bottom >= s.height)
				p.y = s.height - 1.f - r.Height();
			player.SetPosition(p);

			auto rect = this->GetRect();

			for(auto it = enemy_bullets.begin(); it != enemy_bullets.end();){
				(*it)->Move(player);
				if((*it)->IsColliding(player)){
					Gameover();
					return;
				}

				float r = (*it)->GetRadius();
				if(!(*it)->GetCenter().IsInside(rect - Aliases::Point(r, r) + Aliases::Size(r, r))){
					delete *it;
					it =  enemy_bullets.erase(it);
					continue;
				}
				it++;
			}

			for(auto it = enemies.begin(); it != enemies.end();){
				(*it)->Move(player, enemy_bullets);

				if((*it)->IsColliding(player)){
					Gameover();
					return;
				}

				bool erased = false;
				for(auto &b: bullets){
					if(b && (*it)->IsColliding(b)){
						b.SetActive(false);
						delete *it;
						it = enemies.erase(it);
						erased = true;
						break;
					}
				}
				if(erased)
					continue;
				++it;
			}
		}
		void Draw(const Aliases::PaintStruct &ps) override
		{
			this->Base::Draw(ps);

			for(auto &b: bullets){
				if(b){
					b.Draw(ps, holder->yellow_brush);
				}
			}

			player.Draw(ps);

			for(auto &b: enemy_bullets){
				b->Draw(ps);
			}

			for(auto &e: enemies){
				e->Draw(ps);
			}
		}
	};

}
