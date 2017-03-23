#include <SFML\Graphics.hpp>
#include "Vector2.h"
#include <cmath>
#include <iostream>

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

	float angle;
	float angularVelocity;
	int type;
	void updateAngle(float dt)
	{
		angle += angularVelocity*dt;
	}
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
			b.updateAngle(dt);
		}

		if (!balls.empty())
			for (int i = 0; i < balls.size(); i++)
			{
				if (balls[i].position.x >= size.x - balls[i].radius && balls[i].velocity.x > 0)
				{
					float dpn = (2 * (balls[i].velocity) /
						(1 / balls[i].mass)*Vector2(1, 0));
					float dpt = ((balls[i].velocity)*Vector2(1, 0).perpendicular()
						- balls[i].angularVelocity*balls[i].radius) /
						(3 / balls[i].mass);
					Vector2 dp = dpn*Vector2(1, 0) + dpt*Vector2(1, 0);
					balls[i].velocity -= dp / balls[i].mass;
					balls[i].angularVelocity += 2 * dpt / balls[i].mass / balls[i].radius;
				}
				if (balls[i].position.x <= balls[i].radius && balls[i].velocity.x < 0)
				{
					float dpn = (2 * (balls[i].velocity) /
						(1 / balls[i].mass)*Vector2(-1, 0));
					float dpt = ((balls[i].velocity)*Vector2(-1, 0).perpendicular()
						- balls[i].angularVelocity*balls[i].radius) /
						(3 / balls[i].mass);
					Vector2 dp = dpn*Vector2(-1, 0) + dpt*Vector2(-1, 0);
					balls[i].velocity -= dp / balls[i].mass;
					balls[i].angularVelocity += 2 * dpt / balls[i].mass / balls[i].radius;
				}
				if (balls[i].position.y >= size.y - balls[i].radius && balls[i].velocity.y > 0)
				{
					float dpn = (2 * (balls[i].velocity) /
						(1 / balls[i].mass)*Vector2(0, 1));
					float dpt = ((balls[i].velocity)*Vector2(0, 1).perpendicular()
						- balls[i].angularVelocity*balls[i].radius) /
						(3 / balls[i].mass);
					Vector2 dp = dpn*Vector2(0, 1) + dpt*Vector2(0, 1);
					balls[i].velocity -= dp / balls[i].mass;
					balls[i].angularVelocity += 2 * dpt / balls[i].mass / balls[i].radius;
				}
				if (balls[i].position.y <= balls[i].radius && balls[i].velocity.y < 0)
				{
					float dpn = (2 * (balls[i].velocity) /
						(1 / balls[i].mass)*Vector2(0, -1));
					float dpt = ((balls[i].velocity)*Vector2(0, -1).perpendicular()
						- balls[i].angularVelocity*balls[i].radius) /
						(3 / balls[i].mass);
					Vector2 dp = dpn*Vector2(0, -1) + dpt*Vector2(0, -1);
					balls[i].velocity -= dp / balls[i].mass;
					balls[i].angularVelocity += 2 * dpt / balls[i].mass / balls[i].radius;
				}

				for (int i = 0; i < balls.size(); i++)
					for (int j = i + 1; j < balls.size(); j++)
					{
						Vector2 d = balls[j].position - balls[i].position;
						if(d.len() <= balls[j].radius + balls[i].radius)
							if ((balls[j].velocity - balls[i].velocity)*d <= 0)
							{
								//float Ii = balls[i].mass*pow(balls[i].radius, 2) / 2;
								//float Ij = balls[j].mass*pow(balls[j].radius, 2) / 2;
								float dpn = (2 * (balls[i].velocity - balls[j].velocity) /
									(1 / balls[j].mass + 1 / balls[i].mass)*d.norm());
								float dpt = ((balls[i].velocity - balls[j].velocity)*d.norm().perpendicular()
									- balls[i].angularVelocity*balls[i].radius
									- balls[j].angularVelocity*balls[j].radius) /
									(3 / balls[j].mass + 3 / balls[i].mass);
								Vector2 dp = dpn*d.norm() + dpt*d.norm().perpendicular();
								balls[j].velocity += dp / balls[j].mass;
								balls[i].velocity -= dp / balls[i].mass;
								balls[i].angularVelocity += 2 * dpt / balls[i].mass / balls[i].radius;
								balls[j].angularVelocity += 2 * dpt / balls[j].mass / balls[j].radius;
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
			ball.velocity = Vector2(cos(angle), sin(angle)).norm()*(rand() % 300 + 50);
			//ball.velocity = Vector2(0,0);
			ball.angle = 0;
			ball.angularVelocity = 0;
			ball.angularVelocity = rand() % 200 - 100;
			map.balls.push_back(ball);
		}

		if (!map.balls.empty())
			for (int i = 0; i < map.balls.size(); i++)
			{
				balls[i].setPosition(map.balls[i].position.x, 
					map.balls[i].position.y);
				texture.setSmooth(1);
				balls[i].setTexture(texture);
				balls[i].setOrigin(map.balls[i].radius, map.balls[i].radius);
				balls[i].setRotation(-map.balls[i].angle);
				window.draw(balls[i]);
			}



		map.update(dt);
		window.display();
		preTime = time.asSeconds();
	}

	return 0;
}
