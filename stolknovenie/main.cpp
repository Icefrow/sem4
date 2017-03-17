#include <SFML\Graphics.hpp>
#include "Vector2.h"
#include <cmath>
#include <iostream>

/*
не пишите транслитом ... collision, deadline
*/

class MaterialPoint
{
public:
	Vector2 position;
	Vector2 velocity;
	Vector2 acceleration;

	float mass;

	MaterialPoint()
	{
		position = Vector2(0, 0);
		velocity = Vector2(0, 0);
		acceleration = Vector2(0, 0);
		mass = 0.1f;
	}

	void UpdatePosition(float dt)
	{
		/*
		fixit: пробелы вокруг бинарных операторов
		*/
		position += velocity*dt;
		velocity += acceleration*dt;
	}

	Vector2 GetImpulse() const
	{
		return (velocity*mass);
	}

};

class Ball : public MaterialPoint
{
public:
	float radius = 0;
	int type;
};

struct Map
{
	Vector2 size;
	std::vector<Ball> balls;
	void update(float dt)
	{
		for (auto& b : balls)
		{
			b.UpdatePosition(dt);
		}

		if (!balls.empty())
			for (int i = 0; i < balls.size(); i++)
			{
				if (balls[i].position.x >= size.x - balls[i].radius * 2 && balls[i].velocity.x > 0)
				{
					balls[i].velocity.x = -balls[i].velocity.x;
				}
				if (balls[i].position.x <= 0 && balls[i].velocity.x < 0)
				{
					balls[i].velocity.x = -balls[i].velocity.x;
				}
				if (balls[i].position.y >= size.y - balls[i].radius * 2 && balls[i].velocity.y > 0)
				{
					balls[i].velocity.y = -balls[i].velocity.y;
				}
				if (balls[i].position.y <= 0 && balls[i].velocity.y < 0)
				{
					balls[i].velocity.y = -balls[i].velocity.y;
				}

				for (int i = 0; i < balls.size(); i++)
					for (int j = i + 1; j < balls.size(); j++)
					{
						Vector2 d = balls[j].position - balls[i].position;
						/*
						fixit: у вас же есть метод length у Vector2
						*/
						if(sqrt(d.x*d.x + d.y*d.y) <= balls[j].radius + balls[i].radius)
							if ((balls[j].velocity - balls[i].velocity)*d <= 0)
							{
								Vector2 dp = (2 * (balls[i].velocity - balls[j].velocity) /
									(1 / balls[j].mass + 1 / balls[i].mass)*d.norm())*d.norm();
								balls[j].velocity += dp / balls[j].mass;
								balls[i].velocity -= dp / balls[i].mass;
							}
					}
			}
	}
};

int main()
{
	Map map;
	int x = 0;
	std::cin >> map.size;
	double preTime = 0;
	bool mousePressed = 0;
	sf::Clock clock;
	sf::Texture texture;
	texture.loadFromFile("fire.png");
	Ball ball;
	sf::Sprite balls[1000];
	sf::RenderWindow window(sf::VideoMode(map.size.x, map.size.y), "My window");

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
		if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
			mousePressed = 1;
		else if (mousePressed)
		{
			mousePressed = 0;
			ball.radius = texture.getSize().x / 2;
			ball.position = Vector2(sf::Mouse::getPosition(window).x - ball.radius, 
				sf::Mouse::getPosition(window).y - ball.radius);
			float angle = (rand() % 360) / 180.0f * PI;
			ball.velocity = Vector2(cos(angle), sin(angle)).norm()*(rand() % 500 + 400);
			map.balls.push_back(ball);
		}

		if (!map.balls.empty())
			for (int i = 0; i < map.balls.size(); i++)
			{
				balls[i].setPosition(map.balls[i].position.x, 
					map.balls[i].position.y);
				balls[i].setTexture(texture);
				window.draw(balls[i]);
			}



		map.update(dt);
		window.display();
		preTime = time.asSeconds();
	}

	return 0;
}
