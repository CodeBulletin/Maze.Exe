#pragma once
#include "cell.hpp"

#include <random>
#include <iterator>
#include <algorithm>

template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
	std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
	std::advance(start, dis(g));
	return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	return select_randomly(start, end, gen);
}

template<typename Iter>
void randomize(Iter start, Iter end) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::shuffle(start, end, gen);
}


float getRandom() {
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<float> dist(0.0, 1.0);
	return dist(mt);
}

void getNeighbours(cell& Cell, int limitI, int limitJ, std::vector<int>& neighbours, std::vector<std::vector<cell>>& cells) {
	int i = Cell.i;
	int j = Cell.j;
	if (j - 1 >= 0) {
		if (!cells[i][j - 1].visited) {
			neighbours.push_back(0);
		}
	}
	if (i + 1 < limitI) {
		if (!cells[i + 1][j].visited) {
			neighbours.push_back(1);
		}
	}
	if (j + 1 < limitJ) {
		if (!cells[i][j + 1].visited) {
			neighbours.push_back(2);
		}
	}
	if (i - 1 >= 0) {
		if (!cells[i - 1][j].visited) {
			neighbours.push_back(3);
		}
	}
}

void getAllNeighbours(cell& Cell, int limitI, int limitJ, std::vector<int>& neighbours, std::vector<std::vector<cell>>& cells) {
	int i = Cell.i;
	int j = Cell.j;
	if (j - 1 >= 0) {
		neighbours.push_back(0);
	}
	if (i + 1 < limitI) {
		neighbours.push_back(1);
	}
	if (j + 1 < limitJ) {
		neighbours.push_back(2);
	}
	if (i - 1 >= 0) {
		neighbours.push_back(3);
	}
}

void getNeighbourForPathFinding(cell* Cell, int limitI, int limitJ, std::vector<cell*>& neighbours,
	std::vector<std::vector<cell>>& cells) {
	int i = Cell->i;
	int j = Cell->j;
	if (!cells[i][j].walls[0].isActive) {
		neighbours.push_back(&cells[i][j - 1]);
	}
	if (!cells[i][j].walls[1].isActive) {
		neighbours.push_back(&cells[i + 1][j]);
	}
	if (!cells[i][j].walls[2].isActive) {
		neighbours.push_back(&cells[i][j + 1]);
	}
	if (!cells[i][j].walls[3].isActive) {
		neighbours.push_back(&cells[i - 1][j]);
	}
}

float dist(float x1, float y1, float x2, float y2) {
	return std::abs(x1 - x2) + std::abs(y1 - y2);
}

sf::Color HSV_TO_RGB(float H, float S, float V) {
	H = std::fmod(H, 360.0f);
	float C = V * S;
	float X = (1.0f - std::fabs(std::fmod(H / 60.0f, 2.0f) - 1.0f));
	float M = V - C;

	int n = (int)std::floor(H / 60.0f);
	switch (n)
	{
	case 0: return sf::Color((C + M) * 255, (X + M) * 255, (M) * 255, 255); break;
	case 1: return sf::Color((X + M) * 255, (C + M) * 255, (M) * 255, 255); break;
	case 2: return sf::Color((M) * 255, (C + M) * 255, (X + M) * 255, 255); break;
	case 3: return sf::Color((M) * 255, (X + M) * 255, (C + M) * 255, 255); break;
	case 4: return sf::Color((X + M) * 255, (M) * 255, (C + M) * 255, 255); break;
	case 5: return sf::Color((C + M) * 255, (M) * 255, (X + M) * 255, 255); break;
	default: return sf::Color(255, 255, 255, 255);
	}
}
