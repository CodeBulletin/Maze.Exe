#pragma once
#include "wall.hpp"
struct cell {
	int i, j;
	sf::Vector2f pos;
	float size, wallSize;
	sf::Color col, visitedCol, activeCol, currentCol, walkColor, PathColor, frontColor, wallActiveCol, cCol = { 0, 0, 0, 0 };
	bool visited = false, inStack = false, current=false;
	sf::Vertex v[4];
	wall walls[4];
	cell() {}
	cell(int i, int j, const sf::Vector2f& pos, float size, float wallSize, const sf::Color& col, const sf::Color& visitedCol,
		const sf::Color stack_col, const sf::Color& current, const sf::Color& walkColor, const sf::Color& PathColor,
		const sf::Color& frontColor, const sf::Color& wallActiveCol) :
		i(i), j(j), pos(pos), size(size), wallSize(wallSize), col(col), visitedCol(visitedCol), activeCol(stack_col),
		currentCol(current), walkColor(walkColor), PathColor(PathColor), frontColor(frontColor), wallActiveCol(wallActiveCol) {
		makeSquare();
	}

	void makeSquare() {
		v[0].color = col;
		v[0].position = { pos.x - size / 2, pos.y - size / 2 };
		v[1].color = col;
		v[1].position = { pos.x + size / 2, pos.y - size / 2 };
		walls[0] = wall(
			{ v[0].position.x + wallSize, v[0].position.y },
			{ v[1].position.x - wallSize, v[1].position.y },
			{ v[1].position.x - wallSize, v[1].position.y - wallSize },
			{ v[0].position.x + wallSize, v[0].position.y - wallSize },
			wallActiveCol);
		v[2].color = col;
		v[2].position = { pos.x + size / 2, pos.y + size / 2 };
		walls[1] = wall(
			{ v[1].position.x, v[1].position.y + wallSize },
			{ v[2].position.x, v[2].position.y - wallSize },
			{ v[2].position.x + wallSize, v[2].position.y - wallSize },
			{ v[1].position.x + wallSize, v[1].position.y + wallSize },
			wallActiveCol);
		v[3].color = col;
		v[3].position = { pos.x - size / 2, pos.y + size / 2 };
		walls[2] = wall(
			{ v[2].position.x - wallSize, v[2].position.y },
			{ v[3].position.x + wallSize, v[3].position.y },
			{ v[3].position.x + wallSize, v[3].position.y + wallSize },
			{ v[2].position.x - wallSize, v[2].position.y + wallSize },
			wallActiveCol);
		walls[3] = wall(
			{ v[3].position.x, v[3].position.y - wallSize },
			{ v[0].position.x, v[0].position.y + wallSize },
			{ v[0].position.x - wallSize, v[0].position.y + wallSize },
			{ v[3].position.x - wallSize, v[3].position.y - wallSize },
			wallActiveCol);
	}

	void draw(std::vector<sf::Vertex>& drawVec, std::vector<sf::Vertex>& drawWall) {
		drawVec.push_back(v[0]);
		drawVec.push_back(v[1]);
		drawVec.push_back(v[2]);
		drawVec.push_back(v[3]);
		walls[0].draw(drawWall);
		walls[1].draw(drawWall);
		walls[2].draw(drawWall);
		walls[3].draw(drawWall);
	}

	void setVisited() {
		v[0].color = visitedCol;
		v[1].color = visitedCol;
		v[2].color = visitedCol;
		v[3].color = visitedCol;
	}
	void setInStack() {
		v[0].color = activeCol;
		v[1].color = activeCol;
		v[2].color = activeCol;
		v[3].color = activeCol;
	}

	void setCurrent() {
		v[0].color = currentCol;
		v[1].color = currentCol;
		v[2].color = currentCol;
		v[3].color = currentCol;
	}

	void setWalk() {
		v[0].color = walkColor;
		v[1].color = walkColor;
		v[2].color = walkColor;
		v[3].color = walkColor;
	}

	void setPath() {
		v[0].color = PathColor;
		v[1].color = PathColor;
		v[2].color = PathColor;
		v[3].color = PathColor;
	}

	void setFront() {
		v[0].color = frontColor;
		v[1].color = frontColor;
		v[2].color = frontColor;
		v[3].color = frontColor;
	}

	void setColor(const sf::Color& Col) {
		v[0].color = Col;
		v[1].color = Col;
		v[2].color = Col;
		v[3].color = Col;
		cCol = Col;
	}

	void addWalls(std::vector<sf::Vector3i>& WallsList, int dont = -1) {
		if (dont != 3) {
			WallsList.push_back({ i, j, 3 });
		}
		if (dont != 2) {
			WallsList.push_back({ i, j, 2 });
		}
		if (dont != 1) {
			WallsList.push_back({ i, j, 1 });
		}
		if (dont != 0) {
			WallsList.push_back({ i, j, 0 });
		}
	}
};