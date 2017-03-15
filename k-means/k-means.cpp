#include <SFML\Graphics.hpp>
#include <iostream>
#include <cmath>

const int K = 6;
const int IterationCount = 2;

using Centroids = std::vector<sf::Color>;
using ClosestCentroidIndices = std::vector<int>;

struct ColorBuf
{
	int r, g, b, a;
	int count;
	ColorBuf()
	{
		r = 0;
		g = 0;
		b = 0;
		a = 0;
		count = 0;
	}
	
	ColorBuf& operator += (const sf::Color& c)
	{
		count++;
		r += c.r;
		g += c.g;
		b += c.b;
		a += c.a;
		return *this;
	}

	void normalize()
	{
		r = r / count;
		g = g / count;
		b = b / count;
		count = 1;
	}
};

Centroids initializeCentroids(sf::Image& im, const int k)
{
	Centroids centroids(k);
	sf::Vector2u size = im.getSize();
	for (int i = 0; i < k; i++)
	{
		centroids[i] = im.getPixel(rand() % size.x, rand() % size.y);
	}
	return centroids;
}

float distance(sf::Color col, sf::Color centr)
{
	return(sqrt(pow(col.r - centr.r, 2) + pow(col.g - centr.g, 2) + pow(col.b - centr.b, 2)));
}

ClosestCentroidIndices findClosestCentroids(sf::Image& im, Centroids& centroids)
{
	sf::Vector2u size = im.getSize();
	ClosestCentroidIndices ids(size.x*size.y);

	for (int i = 0; i < ids.size(); i++)
	{
		int y = i / size.x;
		int x = i - y*size.x;
		float min = 500;

		for (int j = 0; j < centroids.size(); j++)
			if (min > distance(im.getPixel(x, y), centroids[j]))
			{
				min = distance(im.getPixel(x, y), centroids[j]);
				ids[i] = j;
			}
	}

	return ids;
}


Centroids computeMeans(sf::Image& im, ClosestCentroidIndices& ids, const int k)
{
	sf::Vector2u size = im.getSize();
	ColorBuf buf[K];
	Centroids centroids(k);
	
	for (int i = 0; i < ids.size(); i++)
	{
		int y = i / size.x;
		int x = i - y*size.x;
		buf[ids[i]] += im.getPixel(x, y);
	}

	for (int i = 0; i < k; i++)
	{
		buf[i].normalize();
		centroids[i].r = buf[i].r;
		centroids[i].g = buf[i].g;
		centroids[i].b = buf[i].b;
	}
	return centroids;
}

void changeColors(sf::Image& im, Centroids& centroids)
{
	sf::Color col;
	sf::Vector2u size = im.getSize();
	for (int i = 0; i < size.x*size.y; i++)
	{
		int y = i / size.x;
		int x = i - y*size.x;
		float min = 500;
		col = im.getPixel(x, y);
		for (int j = 0; j < centroids.size(); j++)
			if (min > distance(col, centroids[j]))
			{
				min = distance(col, centroids[j]);
				im.setPixel(x, y, centroids[j]);
			}
	}

}

int main()
{

	sf::Image image;
	image.loadFromFile("hero.png");

	sf::Vector2u size = image.getSize();

	sf::RenderWindow window(sf::VideoMode(size.x, size.y), "My window");

	Centroids centroids = initializeCentroids(image, K);

	for (int it = 0; it < IterationCount; it++)
	{
		ClosestCentroidIndices ids = findClosestCentroids(image, centroids);
		centroids = computeMeans(image, ids, K);
	}

	changeColors(image, centroids);
	sf::Texture texture;
	texture.loadFromImage(image);
	sf::Sprite photo(texture);

	image.saveToFile("result.png");

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear();
		window.draw(photo);
		window.display();
	}
	return 0;
}
