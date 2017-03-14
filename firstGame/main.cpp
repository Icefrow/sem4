#include <SFML\Graphics.hpp>
#include <cmath>
#include <iostream>
#include "Vector2.h"

/*
Герой выпускает шарики только когда запускаешь в Release и непосредственно из bin, 
это связано со строчками 105-109 только я еще не понял как. В остальном программа работает
идеально, скорость очереди выставляется параметром map.delay.velocity
В структуре Images я описал резкое затухание картинки update1 (которое не использовал)
и плавное затухание update2 которое я использовал для молнии (лазера)
*/

struct Hero
{
	Vector2 pos; 
	Vector2 velocity;
	float radius;
	Vector2 direction; 

	void update(float dt)
	{
		pos += velocity * dt;
	}
};

struct Bullet
{
	Vector2 pos;
	Vector2 velocity;
	Vector2 radius;

	void update(float dt) 
	{
		pos += velocity * dt;
	}
};

struct Images
{
	Vector2 pos;
	float opasity = 0;
	float speedOpasity;
	float x;

	void update1(float dt)
	{
		x -= speedOpasity*dt;
		if (x < 0)
			opasity = 0;
	}

	void update2(float dt)
	{
		if (opasity > 0)
			opasity -= speedOpasity*dt;
	}

};

struct Delay
{
	float del;
	float velocity;
	bool isDelay;
	void update(float dt)
	{
		if (del < 1)
		{
			del += velocity*dt;
			isDelay = 1;
		}
		else
		{
			isDelay = 0;
			del = 0;
		}
	}
};

struct Map
{
	Vector2 size;
	Hero hero;
	std::vector<Bullet> bullets;
	Images lightning;
	Images eyes;
	Images eye1;
	Delay delay;

	void update(float dt)
	{
		hero.update(dt);
		for (auto& b : bullets)
		{
			b.update(dt);
		}
		lightning.update2(dt);
		delay.update(dt);

		if (hero.pos.x > size.x)
			hero.pos = Vector2(0, hero.pos.y);
		if (hero.pos.x < 0)
			hero.pos = Vector2(size.x, hero.pos.y);
		if (hero.pos.y > size.y)
			hero.pos = Vector2(hero.pos.x, 0);
		if (hero.pos.y < 0)
			hero.pos = Vector2(hero.pos.x, size.y);
		if (!bullets.empty())
			for (auto itr = bullets.begin(); itr != bullets.end(); )
				if (itr->pos.x > size.x + 100 || itr->pos.x < -100 || itr->pos.y > size.y + 100 || itr->pos.y < -100)
					bullets.erase(itr);
				else ++itr;
	}
};

int main()
{
	Map map;
	std::cin >> map.size;
	map.delay.velocity = 6;
	Bullet blt;
	double preTime = 0;
	float speedEye1 = 0;
	int N = 3;
	bool delay;
	sf::Clock clock;
	sf::RenderWindow window(sf::VideoMode(map.size.x, map.size.y), "My window");
	sf::Texture texture;
	sf::Texture textureLightning;
	textureLightning.loadFromFile("lightning.png");
	sf::Texture textureEyes;
	textureEyes.loadFromFile("eye.png");
	sf::Texture textureEye1;
	textureEye1.loadFromFile("eye1.png");
	sf::Texture textureBullet;
	textureBullet.loadFromFile("fire.png");
	sf::Vector2f pos;
	texture.loadFromFile("sasuke1.png");
	sf::Sprite circle(texture);
	sf::Sprite lightning(textureLightning);
	sf::Sprite eyes(textureEyes);
	sf::Sprite eye1(textureEye1);
	sf::Sprite bullet[1000];
	sf::Vector2u size = texture.getSize();

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		sf::Time time = clock.getElapsedTime();
		double dt = time.asSeconds() - preTime;

		window.clear(sf::Color::Black);
		window.draw(eye1);
		window.draw(eyes);
		


		map.hero.velocity = Vector2(0, 0);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			map.hero.velocity += Vector2(-1000, 0);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			map.hero.velocity += Vector2(1000, 0);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			map.hero.velocity += Vector2(0, -1000);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			map.hero.velocity += Vector2(0, 1000);


		sf::Vector2u circleSize = circle.getTexture()->getSize();
		circle.setOrigin(circleSize.x / 2, circleSize.y / 2);

		sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
		map.hero.direction = Vector2(mousePosition.x, mousePosition.y) - map.hero.pos;


		if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
		{
			map.lightning.opasity = 255;
			map.lightning.speedOpasity = 400;
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			if (!map.delay.isDelay)
			{
				blt.pos = map.hero.pos + map.hero.direction.norm() * 100;
				blt.velocity = map.hero.direction.norm() * 700;
				map.bullets.push_back(blt);
			}
			
		}

		std::cout << map.bullets.size() << std::endl;
		if (!map.bullets.empty())
			for (int i = 0; i < map.bullets.size(); i++)
			{
				bullet[i].setPosition(map.bullets[i].pos.x, map.bullets[i].pos.y);
				bullet[i].setTexture(textureBullet);
				window.draw(bullet[i]);
			}

		circle.setPosition(map.hero.pos.x, map.hero.pos.y);
		circle.setRotation(50 + atan2f(map.hero.direction.y, map.hero.direction.x) * 180 / PI);

		lightning.setColor(sf::Color(255,255,255, map.lightning.opasity));
		lightning.setPosition(map.hero.pos.x, map.hero.pos.y);
		lightning.setRotation(-30 + atan2f(map.hero.direction.y, map.hero.direction.x) * 180 / PI);

		//eyes.setColor(sf::Color(255, 255, 255, 255));
		eyes.setPosition((-1300 + map.size.x) / 2, -100 + (-600 + map.size.y) / 2);

		//eye1.setColor(sf::Color(255, 255, 255, 255));
		eye1.setPosition(1013 + (-1300 + map.size.x) / 2, 275 + (-600 + map.size.y) / 2);
		sf::Vector2u eye1Size = eye1.getTexture()->getSize();
		eye1.setOrigin(eye1Size.x / 2, eye1Size.y / 2);
		eye1.setRotation(speedEye1+=300*dt);

		map.update(dt);
		window.draw(circle);
		window.draw(lightning);
		window.display();
		preTime = time.asSeconds();
	}
	return 0;
}
