#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
struct wall
{
	sf::Vector2f v1, v2, v3, v4;
	sf::Color active;
	bool isActive = true;
	sf::Vertex v[4];
	wall() {}
	wall(sf::Vector2f v1, sf::Vector2f v2, sf::Vector2f v3, sf::Vector2f v4, const sf::Color& active) :
		v1(v1), v2(v2), v3(v3), v4(v4), active(active)
	{
		v[0].position = v1;
		v[1].position = v2;
		v[2].position = v3;
		v[3].position = v4;
		v[0].color = active;
		v[1].color = active;
		v[2].color = active;
		v[3].color = active;
	}

	void draw(std::vector<sf::Vertex>& drawVec) {
		if (isActive) {
			drawVec.push_back(v[0]);
			drawVec.push_back(v[1]);
			drawVec.push_back(v[2]);
			drawVec.push_back(v[3]);
		}
	}

	void Deactivate() {
		isActive = false;
	}
};