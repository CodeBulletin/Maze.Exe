#pragma once
#include "cell.hpp"

struct Player{
	sf::Vector2i targetPos, playerPos, target, player;
	float tsize, psize;
	sf::Color targetColor, playerColor;
	sf::VertexArray t_vert, p_vert;
	Player() {}
	Player(sf::Vector2i targetPos, sf::Vector2i playerPos, sf::Vector2i target, sf::Vector2i player,
		float tsize, float psize, const sf::Color& targetColor, const sf::Color& playerColor) :
		targetPos(targetPos), playerPos(playerPos), target(target), player(player),
		tsize(tsize), psize(psize), targetColor(targetColor), playerColor(playerColor){

		t_vert = sf::VertexArray(sf::Quads, 8);
		p_vert = sf::VertexArray(sf::Quads, 4);

		t_vert[0].color = targetColor;
		t_vert[1].color = targetColor;
		t_vert[2].color = targetColor;
		t_vert[3].color = targetColor;
		t_vert[4].color = targetColor;
		t_vert[5].color = targetColor;
		t_vert[6].color = targetColor;
		t_vert[7].color = targetColor;
		t_vert[0].position = { 0.5f * tsize + targetPos.x, tsize + targetPos.y };
		t_vert[1].position = { tsize + targetPos.x, 0.5f * tsize + targetPos.y };
		t_vert[2].position = { -0.5f * tsize + targetPos.x, -tsize + targetPos.y };
		t_vert[3].position = { -tsize + targetPos.x, -0.5f * tsize + targetPos.y };
		t_vert[4].position = { -0.5f * tsize + targetPos.x, tsize + targetPos.y };
		t_vert[5].position = { -tsize + targetPos.x, 0.5f * tsize + targetPos.y };
		t_vert[6].position = { 0.5f * tsize + targetPos.x, -tsize + targetPos.y };
		t_vert[7].position = { tsize + targetPos.x, -0.5f * tsize + targetPos.y };
		calPlayer();
	}

	void calPlayer() {
		p_vert[0].color = playerColor;
		p_vert[1].color = playerColor;
		p_vert[2].color = playerColor;
		p_vert[3].color = playerColor;
		p_vert[0].position = { playerPos.x - psize / 2, playerPos.y - psize / 2 };
		p_vert[1].position = { playerPos.x + psize / 2, playerPos.y - psize / 2 };
		p_vert[2].position = { playerPos.x + psize / 2, playerPos.y + psize / 2 };
		p_vert[3].position = { playerPos.x - psize / 2, playerPos.y + psize / 2 };
	}

	bool win() {
		return (target.x == player.x && target.y == player.y);
	}

	void draw(sf::RenderWindow& win) {
		win.draw(t_vert);
		win.draw(p_vert);
	}

	void draw(sf::RenderWindow& win, sf::Shader* shader) {
		win.draw(t_vert, shader);
		win.draw(p_vert, shader);
	}

	void setPlayer(cell& Cell) {
		player = { Cell.i, Cell.j };
		playerPos = { (int)Cell.pos.x, (int)Cell.pos.y };
		calPlayer();
	}

	void playerInput(int dir, std::vector<std::vector<cell>>& cells) {
		cell& Cell = cells[player.x][player.y];
		cells[player.x][player.y].setWalk();
		if (dir == 0 && !Cell.walls[dir].isActive) {
			if (player.y - 1 >= 0) {
				cells[player.x][player.y - 1].setWalk();
				setPlayer(cells[player.x][player.y - 1]);
			}
		}
		else if (dir == 1 && !Cell.walls[dir].isActive) {
			if (player.x + 1 < cells.size()) {
				cells[player.x + 1][player.y].setWalk();
				setPlayer(cells[player.x + 1][player.y]);
			}
		}
		else if (dir == 2 && !Cell.walls[dir].isActive) {
			if (player.y + 1 < cells[0].size()) {
				cells[player.x][player.y + 1].setWalk();
				setPlayer(cells[player.x][player.y + 1]);
			}
		}
		else if (dir == 3 && !Cell.walls[dir].isActive) {
			if (player.x - 1 >= 0) {
				cells[player.x - 1][player.y].setWalk();
				setPlayer(cells[player.x - 1][player.y]);
			}
		}
	}
};