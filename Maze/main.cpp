#include "DisJointSet.hpp"
#include "player.hpp"
#include <queue>
#include <unordered_map>
#include <Windows.h>
#include "SFML/Audio.hpp"

//Options
int Method = 0;
int StartMethod = 1;
//Switch
bool instaComplete = true;
bool PrintMode = false;
bool ShowPath = false;
bool ShowPathAnimation = false;
bool showScore = true;
bool SimPlay = true;
bool ScoreMode = true;
//Sliders
int Difficulty = 1;
int CellSize = 0;
float straightness = 0.5;


static const std::string name = "Maze";
static int width;
static int height;

void setup();
void loop(sf::RenderWindow&, sf::Shader&);
void eventManager(sf::Event&, sf::RenderWindow&, sf::Music&);
void keyboardeEvent(sf::Event&, sf::RenderWindow&, sf::Music&);
void mouseEvent(sf::Event&);
void RandomeDepthFirstSearch();
void PrimsAlgorithm();
void AldousBroder();
void KruskalsAlgorithm();
void AstarPathFinding();
void recreatePath();

std::vector<sf::Vertex> vertArray, wallVertArray;
int wCells, hCells;
float wPosAdd, hPosAdd, CellHalfSize, fCellSize;
int initialI = 0, finalI = 0;
int initialJ = 0, finalJ = 0;
bool completed = false;
int direction = -1;
sf::Vector2i Cindex;
bool Continue = false;
int largest = 0;
Player player;
bool gotPath = false, pathFound = false;
int PathIndex = 0;
bool hasWon = false;
bool mainMenu = true;

int Score = 0;
float timeLeft = 0;
int AddScore = 0;

int CellSizeDiff[] = { 120, 100, 80, 60, 40, 30, 20 };
int ScoreAdd[] = { 4, 6, 8, 12, 16, 40, 80 };
float timeGiven[] = { 15.0f, 20.0f, 30.0f, 50.0f, 100.0f, 200.0f, 360.0f };

sf::Vector3i directionMap[4] = { {0, -1, 0}, {1, 0, 1}, {0, 1, 2}, {-1, 0, 3} };
int QuadMap[4] = { 2, 3, 0, 1 };

std::vector<std::vector<cell>> cells;
std::vector<cell*> pStack;
std::vector<cell*> tStack;
std::vector<sf::Vector3i> pWall;
std::vector<sf::Vector2i> indexes;
DisjSet ds;
cell* current, *Acurrent;
std::vector<cell*> OpenSet;
std::unordered_map<cell*, cell*> CameFrom;
std::unordered_map<cell*, float> hScore, gScore, fScore;
cell* goal;
bool found = false;

sf::VertexArray VCC;


std::string musicFiles[2] = { "data/A.wav", "data/B.wav" };

std::string shaderCode = "\
#version 130 \n\
\n\
in vec4 gl_FragCoord;\n\
\n\
out vec4 FragColor;\n\
\n\
uniform int col;\n\
\n\
void main() {\n\
	if (col == 1) {\n\
		FragColor = vec4(0.0, 0.0, 0.0, 1.0);\n\
	}\n\
	else {\n\
		FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n\
	}\n\
}\
";

int main() {
	FreeConsole();
	sf::Shader bShader;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	auto vid = sf::VideoMode::getDesktopMode();
	sf::RenderWindow window;
	window.create(vid, name, sf::Style::Fullscreen, settings);

	width = vid.width;
	height = vid.height;

	sf::Event event;
	window.setVerticalSyncEnabled(true);
	bShader.loadFromMemory(shaderCode, sf::Shader::Fragment);
	sf::Image image;
	image.loadFromFile("data/MAZE.png");
	window.setIcon(image.getSize().x, image.getSize().y, image.getPixelsPtr());

	//window.setFramerateLimit(24);

	sf::Font font;
	font.loadFromFile("data/font.ttf");
	sf::Text text, text2;
	text.setFont(font);
	text.setFillColor(sf::Color::White);
	text.setOutlineColor(sf::Color::Black);
	text2.setFont(font);
	text2.setFillColor(sf::Color::White);
	text2.setOutlineColor(sf::Color::Black);
	int mI = 0;

	sf::Music music;
	setup();

	sf::Clock clock;
	while (window.isOpen()) {
		sf::Time time = clock.restart();
		while (window.pollEvent(event)) {
			eventManager(event, window, music);
		}
		if (mainMenu) {
			text.setCharacterSize(128);
			text2.setCharacterSize(32);
			text.setString("MAZE.EXE");
			text2.setString("Press Enter to Play");
			sf::FloatRect textRect = text.getLocalBounds();
			text.setOrigin(textRect.left + textRect.width / 2.0f,
				textRect.top + textRect.height / 2.0f);
			text.setPosition(sf::Vector2f(width / 2, height / 3.0f));
			textRect = text2.getLocalBounds();
			text2.setOrigin(textRect.left + textRect.width / 2.0f,
				textRect.top + textRect.height / 2.0f);
			text2.setPosition(sf::Vector2f(width / 2, 3 * height / 4.0f));
		} 
		else if (showScore) {
			text.setCharacterSize(32);
			text2.setCharacterSize(32);
			std::string scoreString = "Score:" + std::to_string(Score);
			std::string num_text = std::to_string(timeLeft);
			std::string rounded = num_text.substr(0, num_text.find(".") + 3);
			std::string timeString = "Time Left:" + rounded;
			text.setString(scoreString);
			text2.setString(timeString);
			sf::FloatRect textRect = text.getLocalBounds();
			text.setOrigin(textRect.left + textRect.width / 2.0f,
				textRect.top + textRect.height / 2.0f);
			text.setPosition(sf::Vector2f(width / 2, 40));
			textRect = text2.getLocalBounds();
			text2.setOrigin(textRect.left + textRect.width / 2.0f,
				textRect.top + textRect.height / 2.0f);
			text2.setPosition(sf::Vector2f(width / 2, 80));
		}
		if (music.getStatus() == sf::Sound::Stopped) {
			music.openFromFile(musicFiles[mI % 2]);
			music.play();
			mI += 1;
		}
		window.clear(sf::Color::Black);
		if (!mainMenu) {
			loop(window, bShader);
		}
		if (completed && ScoreMode) {
			if (player.win() && !hasWon) {
				if (timeLeft >= timeGiven[Difficulty] * 0.5f) {
					Score += (int)((float)AddScore * 1.5f);
				}
				else if (timeLeft >= 0) {
					Score += AddScore;
				}
				else {
					Score += (int)((float)AddScore * 0.75f);
				}
				hasWon = true;
			}
			if(!player.win()) {
				timeLeft -= time.asSeconds();
			}
		}
		if ((showScore && completed && ScoreMode) || mainMenu) {
			window.draw(text);
			window.draw(text2);
		}
		window.display();
	}

	return 0;
}

void setup() {
	if (ScoreMode) {
		Method = getRandom() * 4;
		straightness = (int)(getRandom() * 51) / 100.0f;
		AddScore = ScoreAdd[Difficulty];
		timeLeft = timeGiven[Difficulty];
		CellSize = CellSizeDiff[Difficulty];
		StartMethod = 1;
		if (Method == 0 && getRandom() < 0.5f) StartMethod = 3;
	}
	tStack.clear();
	OpenSet.clear();
	CameFrom.clear();
	hScore.clear();
	gScore.clear();
	fScore.clear();
	current = NULL;
	Acurrent = NULL;
	player = Player();
	Continue = false;
	completed = false;
	gotPath = false;
	found = false;
	pathFound = false;
	hasWon = false;
	Continue = (ShowPath || !SimPlay);
	wCells = width / CellSize;
	hCells = height / CellSize;
	largest = 0;
	PathIndex = 0;
	fCellSize = CellSize;
	CellHalfSize = fCellSize / 2.0f;

	wPosAdd = ((float)width / fCellSize - (float)wCells) * CellHalfSize;
	hPosAdd = ((float)height / fCellSize - (float)hCells) * CellHalfSize;
	cells = std::vector<std::vector<cell>>(wCells, std::vector<cell>(hCells));
	for (int i = 0; i < wCells; i++) {
		for (int j = 0; j < hCells; j++) {
			cells[i][j] = cell(i, j, { wPosAdd + fCellSize * i + CellHalfSize, hPosAdd + fCellSize * j + CellHalfSize },
				CellSize, -CellSize / 10, sf::Color::Black, sf::Color(43 * 255 / 100, 98 * 255 / 100, 68 * 255 / 100, 255),
				sf::Color(84 * 255 / 100, 64 * 255 / 100, 40 * 255 / 100, 255), sf::Color(80 * 255 / 100, 255, 55 * 255 / 100),
				sf::Color(61, 252, 3, 255), sf::Color(61, 130, 227), sf::Color(235, 98, 0, 255), sf::Color::Black);
		}
	}
	if (StartMethod == 0) {
		initialI = getRandom() * (float)wCells;
		initialJ = getRandom() * (float)hCells;
		finalI = getRandom() * (float)wCells;
		finalJ = getRandom() * (float)hCells;
		player = Player({ (int)wPosAdd + CellSize * finalI + (int)CellHalfSize, (int)hPosAdd + CellSize * finalJ + (int)CellHalfSize },
			{ (int)wPosAdd + CellSize * initialI + (int)CellHalfSize, (int)hPosAdd + CellSize * initialJ + (int)CellHalfSize },
			{ finalI, finalJ }, { initialI, initialJ }, 8 * CellSize / 20, 3 * CellSize / 4, sf::Color::Red, sf::Color::Magenta);
	}
	else if (StartMethod == 1) {
		initialI = 0;
		initialJ = 0;
		finalI = wCells - 1;
		finalJ = hCells - 1;
		player = Player({ (int)wPosAdd + CellSize * finalI + (int)CellHalfSize, (int)hPosAdd + CellSize * finalJ + (int)CellHalfSize },
			{ (int)wPosAdd + CellSize * initialI + (int)CellHalfSize, (int)hPosAdd + CellSize * initialJ + (int)CellHalfSize },
			{ finalI, finalJ }, { initialI, initialJ }, 8 * CellSize / 20, 3 * CellSize / 4, sf::Color::Red, sf::Color::Magenta);
	}
	else if (StartMethod == 2 && Method == 0) {
		initialI = getRandom() * (float)wCells;
		initialJ = getRandom() * (float)hCells;
	}
	else if (StartMethod == 3 && Method == 0) {
		initialI = 0;
		initialJ = 0;
	}
	else {
		initialI = getRandom() * (float)wCells;
		initialJ = getRandom() * (float)hCells;
		finalI = getRandom() * (float)wCells;
		finalJ = getRandom() * (float)hCells;
		player = Player({ (int)wPosAdd + CellSize * finalI + (int)CellHalfSize, (int)hPosAdd + CellSize * finalJ + (int)CellHalfSize },
			{ (int)wPosAdd + CellSize * initialI + (int)CellHalfSize, (int)hPosAdd + CellSize * initialJ + (int)CellHalfSize },
			{ finalI, finalJ }, { initialI, initialJ }, 8 * CellSize / 20, 3 * CellSize / 4, sf::Color::Red, sf::Color::Magenta);
	}

	if (Method == 0) {
		pStack.clear();
		pStack.push_back(&cells[initialI][initialJ]);
		pStack.back()->visited = true;
		pStack.back()->setVisited();
	}
	if (Method == 1) {
		pWall.clear();
		current = &cells[initialI][initialJ];
		current->visited = true;
		current->setVisited();
		current->addWalls(pWall);
	}
	if (Method == 2) {
		indexes.clear();
		for (int i = 0; i < wCells; i++) {
			for (int j = 0; j < hCells; j++) {
				indexes.push_back({ i, j });
			}
		}
		cells[initialI][initialJ].visited = true;
		cells[initialI][initialJ].setVisited();
		current = &cells[initialI][initialJ];
		Cindex = indexes[initialI * hCells + initialJ];
	}
	if (Method == 3) {
		ds = DisjSet(wCells, hCells);
		for (int i = 0; i < wCells; i++) {
			for (int j = 0; j < hCells; j++) {
				cells[i][j].addWalls(pWall);
			}
		}
		randomize(pWall.begin(), pWall.end());
	}
	VCC = sf::VertexArray(sf::Quads, 16);
	VCC[0].color = sf::Color(32, 26, 43, 255);
	VCC[1].color = sf::Color(32, 26, 43, 255);
	VCC[2].color = sf::Color(32, 26, 43, 255);
	VCC[3].color = sf::Color(32, 26, 43, 255);
	VCC[4].color = sf::Color(32, 26, 43, 255);
	VCC[5].color = sf::Color(32, 26, 43, 255);
	VCC[6].color = sf::Color(32, 26, 43, 255);
	VCC[7].color = sf::Color(32, 26, 43, 255);
	VCC[8].color = sf::Color(32, 26, 43, 255);
	VCC[9].color = sf::Color(32, 26, 43, 255);
	VCC[10].color = sf::Color(32, 26, 43, 255);
	VCC[11].color = sf::Color(32, 26, 43, 255);
	VCC[12].color = sf::Color(32, 26, 43, 255);
	VCC[13].color = sf::Color(32, 26, 43, 255);
	VCC[14].color = sf::Color(32, 26, 43, 255);
	VCC[15].color = sf::Color(32, 26, 43, 255);
	VCC[0].position = { 0.0f, 0.0f };
	VCC[1].position = { (float) width, 0.0f };
	VCC[2].position = { (float)width - wPosAdd, hPosAdd };
	VCC[3].position = { wPosAdd, hPosAdd };
	VCC[4].position = { (float)width, 0.0f };
	VCC[5].position = { (float)width - wPosAdd, hPosAdd };
	VCC[6].position = { (float)width - wPosAdd, (float)height - hPosAdd };
	VCC[7].position = { (float)width, (float)height };
	VCC[8].position = { (float)width, (float)height };
	VCC[9].position = { (float)width - wPosAdd, (float)height - hPosAdd };
	VCC[10].position = { wPosAdd, (float)height - hPosAdd };
	VCC[11].position = { 0.0f, (float)height};
	VCC[12].position = { wPosAdd, (float)height - hPosAdd };
	VCC[13].position = { 0.0f, (float)height };
	VCC[14].position = { 0.0f, 0.0f };
	VCC[15].position = { wPosAdd, hPosAdd };
}

void eventManager(sf::Event& event, sf::RenderWindow& win, sf::Music& music) {
	switch (event.type)
	{
	case sf::Event::KeyPressed:
		keyboardeEvent(event, win, music);
		break;
	case sf::Event::MouseButtonPressed:
		mouseEvent(event);
		break;
	default:
		break;
	}
}

void keyboardeEvent(sf::Event& event, sf::RenderWindow& win, sf::Music& music) {
	if (event.key.code == sf::Keyboard::Escape) {
		win.close();
		exit(0);
	}
	else if (event.key.code == sf::Keyboard::F9) {
		if (music.getStatus() == sf::Sound::Paused) {
			music.play();
		}
		else if (music.getStatus() == sf::Sound::Playing) {
			music.pause();
		}
	}
	else if (!mainMenu) {
		if (completed && SimPlay && (!ShowPath or pathFound)) {
			if (!player.win()) {
				if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) {
					player.playerInput(0, cells);
				}
				else if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right) {
					player.playerInput(1, cells);
				}
				else if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) {
					player.playerInput(2, cells);
				}
				else if (event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left) {
					player.playerInput(3, cells);
				}
			}
		}
		if (event.key.code == sf::Keyboard::Num1 && !ScoreMode) {
			Method = 0;
			setup();
		}
		else if (event.key.code == sf::Keyboard::Num2 && !ScoreMode) {
			Method = 1;
			setup();
		}
		else if (event.key.code == sf::Keyboard::Num3 && !ScoreMode) {
			Method = 2;
			setup();
		}
		else if (event.key.code == sf::Keyboard::Num4 && !ScoreMode) {
			Method = 3;
			setup();
		}
		else if (event.key.code == sf::Keyboard::Num5 && !ScoreMode) {
			StartMethod = 0;
			setup();
		}
		else if (event.key.code == sf::Keyboard::Num6 && !ScoreMode) {
			StartMethod = 1;
			setup();
		}
		else if (event.key.code == sf::Keyboard::Num7 && !ScoreMode) {
			StartMethod = 2;
			setup();
		}
		else if (event.key.code == sf::Keyboard::Num8 && !ScoreMode) {
			StartMethod = 3;
			setup();
		}
		else if (event.key.code == sf::Keyboard::Num0 && !ScoreMode) {
			PrintMode = !PrintMode;
		}
		if (completed && !ScoreMode) {
			if (event.key.code == sf::Keyboard::RBracket) {
				ShowPath = !ShowPath;
				Continue = true;
				if (!ShowPath) {
					for (cell* Cell : tStack) {
						Cell->setVisited();
					}
				}
				else if (pathFound) {
					for (cell* Cell : tStack) {
						Cell->setPath();
					}
				}
			}
			if (event.key.code == sf::Keyboard::LBracket && !ScoreMode) {
				ShowPathAnimation = !ShowPathAnimation;
			}
		}
		if (event.key.code == sf::Keyboard::P && !ScoreMode) {
			instaComplete = !instaComplete;
		}

		if (event.key.code == sf::Keyboard::Tab && !ScoreMode) {
			SimPlay = !SimPlay;
			if (!SimPlay) {
				Continue = true;
			}
		}
		if (event.key.code == sf::Keyboard::Comma && !ScoreMode) {
			if (CellSize > 20) {
				CellSize--;
				setup();
			}
		}
		if (event.key.code == sf::Keyboard::Period && !ScoreMode) {
			if (CellSize < 200) {
				CellSize++;
				setup();
			}
		}
		if (event.key.code == sf::Keyboard::K && !ScoreMode) {
			if (straightness > 0.00f && (Method == 0 || Method == 2)) {
				straightness -= 0.01f;
				setup();
			}
		}
		if (event.key.code == sf::Keyboard::L && !ScoreMode) {
			if (straightness < 1.00f && (Method == 0 || Method == 2)) {
				straightness += 0.01f;
				setup();
			}
		}
		if (Continue) {
			if (event.key.code == sf::Keyboard::Space) {
				setup();
			}
		}
		if (event.key.code == sf::Keyboard::BackSpace) {
			ScoreMode = false;
			setup();
		}
		if (event.key.code == sf::Keyboard::Equal && ScoreMode) {
			Difficulty = (Difficulty + 1) % 7;
			setup();
		}
		if (event.key.code == sf::Keyboard::Tilde && ScoreMode) {
			showScore = !showScore;
		}
	}
	else if(event.key.code == sf::Keyboard::Enter){
		mainMenu = false;
	}
	
}

void mouseEvent(sf::Event& event) {
	if (completed && SimPlay && (!ShowPath or pathFound) && !player.win() && !mainMenu) {
		sf::Vector2i mouse_pos;
		if (event.mouseButton.button == sf::Mouse::Button::Left) {
			mouse_pos = { (int)std::floor((float)(event.mouseButton.x - wPosAdd) / fCellSize),
				(int)std::floor((float)(event.mouseButton.y - hPosAdd) / fCellSize) };
			if (mouse_pos.x >= 0 && mouse_pos.x < wCells && mouse_pos.y >= 0 && mouse_pos.y < hCells) {
				if (mouse_pos.y != player.player.y || mouse_pos.x != player.player.x) {
					int dir = -1, dirX = 0, dirY = 0;
					int indexdiffY = 0;
					int indexdiffX = 0;
					if (mouse_pos.x == player.player.x) {
						indexdiffY = -(mouse_pos.y - player.player.y);
						dir = 0;
						dirX = 1;
					}
					else if (mouse_pos.y == player.player.y) {
						indexdiffX = -(mouse_pos.x - player.player.x);
						dir = 3;
						dirY = 1;
					}
					if (dir != -1) {
						bool hasWall = false;
						auto y = std::minmax(mouse_pos.x + indexdiffX, mouse_pos.x);
						auto x = std::minmax(mouse_pos.y + indexdiffY, mouse_pos.y);
						for (int i = x.first; i <= x.second - dirX; i++) {
							for (int j = y.first; j <= y.second - dirY; j++) {
								if (cells[j][i].walls[QuadMap[dir]].isActive) {
									hasWall = true;
									break;
								}
							}
							if (hasWall) break;
						}
						if (!hasWall) {
							for (int i = x.first; i <= x.second; i++) {
								for (int j = y.first; j <= y.second; j++) {
									cells[j][i].setWalk();
								}
							}
							player.setPlayer(cells[mouse_pos.x][mouse_pos.y]);
						}
					}
				}
			}
		}
	} 
}

void loop(sf::RenderWindow& win, sf::Shader& bShader) {
	do {
		if (Method == 0) {
			RandomeDepthFirstSearch();
		}
		else if (Method == 1) {
			PrimsAlgorithm();
		}
		else if (Method == 2) {
			AldousBroder();
		}
		else if (Method == 3) {
			KruskalsAlgorithm();
		}
	} while (instaComplete && !completed);
	for (std::vector<cell>& row : cells) {
		for (cell& Cell : row) {
			Cell.draw(vertArray, wallVertArray);
		}
	}


	if(!gotPath && completed){
		if (Method == 0 && (StartMethod == 2 or StartMethod == 3)) {
			finalI = tStack.back()->i;
			finalJ = tStack.back()->j;
			player = Player({ (int)wPosAdd + CellSize * finalI + (int)CellHalfSize, (int)hPosAdd + CellSize * finalJ + (int)CellHalfSize },
				{ (int)wPosAdd + CellSize * initialI + (int)CellHalfSize, (int)hPosAdd + CellSize * initialJ + (int)CellHalfSize },
				{ finalI, finalJ }, { initialI, initialJ }, 8 * CellSize / 20, 3 * CellSize / 4, sf::Color::Red, sf::Color::Magenta);
			tStack.clear();
		}
		else if (Method == 0) {
			tStack.clear();
		}
		else {
			for (std::vector<cell>& row : cells) {
				for (cell& Cell : row) {
					Cell.setVisited();
				}
			}
		}
		goal = &cells[player.target.x][player.target.y];
		OpenSet.push_back(&cells[player.player.x][player.player.y]);
		CameFrom.clear();
		gScore.clear();
		hScore.clear();
		fScore.clear();
		for (std::vector<cell>& row : cells) {
			for (cell& Cell : row) {
				gScore.insert({ &Cell, 1000000000.0f });
				fScore.insert({ &Cell, 1000000000.0f });
				hScore.insert({ &Cell, dist(Cell.i, Cell.j, player.target.x, player.target.y) });
				gScore[&cells[player.player.x][player.player.y]] = 0;
				fScore[&cells[player.player.x][player.player.y]] = hScore[&cells[player.player.x][player.player.y]];
			}
		}
		gotPath = true;
	}
	if (gotPath && ShowPath && !pathFound) {
		if (!ShowPathAnimation) {
			if (!found) {
				while (!OpenSet.empty() and !found)
				{
					AstarPathFinding();
				}
				tStack.clear();
				recreatePath();
				for (cell* Cell : tStack) {
					Cell->setPath();
				}
				pathFound = true;
			}
		}
		else {
			if (!found && !OpenSet.empty()) {
				AstarPathFinding();
			}
			else {
				if (PathIndex == 0) {
					tStack.clear();
					recreatePath();
				}
				if (PathIndex < tStack.size()) {
					tStack[PathIndex]->setPath();
				}
				else if (PathIndex == tStack.size()) {
					for (std::vector<cell>& row : cells) {
						for (cell& Cell : row) {
							Cell.setVisited();
						}
					}
					for (cell* Cell : tStack) {
						Cell->setPath();
					}
					pathFound = true;
				}
				PathIndex += 1;
			}
		}
	}
	
	if(!Continue && completed) {
		if (player.win()) {
			Continue = true;
		}
	}
	if (!PrintMode) {
		win.draw(vertArray.data(), vertArray.size(), sf::Quads);
		if (completed) {
			player.draw(win);
		}
		win.draw(wallVertArray.data(), wallVertArray.size(), sf::Quads);
		win.draw(VCC);
	}
	else {
		bShader.setUniform("col", 0);
		win.draw(vertArray.data(), vertArray.size(), sf::Quads, &bShader);
		bShader.setUniform("col", 1);
		if (completed) {
			player.draw(win, &bShader);
		}
		win.draw(wallVertArray.data(), wallVertArray.size(), sf::Quads, &bShader);
		bShader.setUniform("col", 0);
		win.draw(VCC, &bShader);
	}
	vertArray.clear();
	wallVertArray.clear();
}

void RandomeDepthFirstSearch() {
	if (pStack.size() > 0 && !completed) {
		if (pStack.size() > largest) {
			largest = pStack.size();
			tStack = pStack;
		}
		current = pStack.back();
		pStack.pop_back();
		std::vector<int> neighbours;
		getNeighbours(*current, wCells, hCells, neighbours, cells);
		if (neighbours.size() > 0) {
			pStack.push_back(current);
			int dir_choosen = *select_randomly(neighbours.begin(), neighbours.end());
			if (direction == -1 or std::count(neighbours.begin(), neighbours.end(), direction) == 0 or (1 - straightness) > getRandom())
				direction = dir_choosen;
			sf::Vector3i choosen = sf::Vector3i(current->i, current->j, 0) + directionMap[direction];
			cells[choosen.x][choosen.y].visited = true;
			current->walls[choosen.z].Deactivate();
			cells[choosen.x][choosen.y].walls[QuadMap[choosen.z]].Deactivate();
			pStack.push_back(&cells[choosen.x][choosen.y]);
			pStack.back()->visited = true;
			current->setInStack();
			pStack.back()->setCurrent();
		}
		else {
			current->setVisited();
			if (pStack.size() > 0) {
				pStack.back()->setCurrent();
			}
		}
	}
	else {
		completed = true;
	}
}

void PrimsAlgorithm() {
	if (pWall.size() > 0) {
		bool added = false;
		while (!added and pWall.size() > 0) {
			sf::Vector3i Wall = *select_randomly(pWall.begin(), pWall.end());
			sf::Vector3i Dir = directionMap[Wall.z];
			int iC = Wall.x + Dir.x, jC = Wall.y + Dir.y;
			while (not (iC >= 0 and iC < wCells and jC >= 0 and jC < hCells)) {
				pWall.erase(std::find(pWall.begin(), pWall.end(), Wall));
				if (pWall.size() == 0) return;
				Wall = *select_randomly(pWall.begin(), pWall.end());
				Dir = directionMap[Wall.z];
				iC = Wall.x + Dir.x;
				jC = Wall.y + Dir.y;
			}
			cell& Cell = cells[Wall.x][Wall.y];
			cell& nextCell = cells[iC][jC];
			Cell.setInStack();
			nextCell.setInStack();
			if (!Cell.visited or !nextCell.visited) {
				Cell.walls[Wall.z].Deactivate();
				nextCell.walls[QuadMap[Wall.z]].Deactivate();
				nextCell.visited = true;
				nextCell.setCurrent();
				Cell.setVisited();
				nextCell.addWalls(pWall, QuadMap[Wall.z]);
				added = true;
			}
			pWall.erase(std::find(pWall.begin(), pWall.end(), Wall));
		}
	}
	else {
		completed = true;
	}
}

void AldousBroder() {
	bool added = false;
	if (!completed) {
		while (!added && indexes.size() > 0) {
			std::vector<int> neighbours;
			getAllNeighbours(*current, wCells, hCells, neighbours, cells);
			int dirChoosen = *select_randomly(neighbours.begin(), neighbours.end());
			if (direction == -1 or std::count(neighbours.begin(), neighbours.end(), direction) == 0 or (1 - straightness * 0.999f) > getRandom())
				direction = dirChoosen;
			sf::Vector3i choosen = sf::Vector3i(current->i, current->j, 0) + directionMap[direction];
			cells[choosen.x][choosen.y].setVisited();
			if (not cells[choosen.x][choosen.y].visited) {
				cells[choosen.x][choosen.y].visited = true;
				cells[choosen.x][choosen.y].setCurrent();
				current->setInStack();
				current->walls[choosen.z].Deactivate();
				cells[choosen.x][choosen.y].walls[QuadMap[choosen.z]].Deactivate();
				added = true;
			}
			auto it = std::find(indexes.begin(), indexes.end(), Cindex);
			if (it != indexes.end()) {
				indexes.erase(it);
			}
			current = &cells[choosen.x][choosen.y];
			Cindex = { choosen.x, choosen.y };
		}
		if(indexes.size() == 0) {
			completed = true;
		}
	}
	else {
		completed = true;
	}
}

void KruskalsAlgorithm() {
	bool added = false;
	if (!completed) {
		while (pWall.size() > 0 && !added && !completed) {
			sf::Vector3i Wall = pWall.back();
			pWall.pop_back();
			sf::Vector3i Dir = directionMap[Wall.z];
			int iC = Wall.x + Dir.x, jC = Wall.y + Dir.y;
			while (not (iC >= 0 and iC < wCells and jC >= 0 and jC < hCells)) {
				auto it = std::find(pWall.begin(), pWall.end(), Wall);
				if (it != pWall.end()) {
					pWall.erase(it);
				}
				if (pWall.size() == 0) return;
				Wall = *select_randomly(pWall.begin(), pWall.end());
				Dir = directionMap[Wall.z];
				iC = Wall.x + Dir.x;
				jC = Wall.y + Dir.y;
			}
			cell& Cell = cells[Wall.x][Wall.y];
			cell& nextCell = cells[iC][jC];
			if (ds.union_sets(Wall.x * hCells + Wall.y, iC * hCells + jC)) {
				Cell.walls[Wall.z].Deactivate();
				nextCell.walls[QuadMap[Wall.z]].Deactivate();
				nextCell.visited = true;
				added = true;
			}
		}
		if (pWall.size() == 0) {
			completed = true;
		}
		ds.setSameColor(cells);
	}
}

void recreatePath() {
	tStack.push_back(Acurrent);
	while (Acurrent != NULL) {
		Acurrent = CameFrom[Acurrent];
		if (Acurrent != NULL) {
			tStack.push_back(Acurrent);
		}
	}
}

void AstarPathFinding() {
	if (!OpenSet.empty() && !found) {
		if (ShowPathAnimation && Acurrent != NULL) {
			Acurrent->setInStack();
		}
		int least = 1000000000.0f;
		for (cell* Cell : OpenSet) {
			float f = fScore[Cell];
			if (f < least) {
				Acurrent = Cell;
				least = f;
			}
			if (ShowPathAnimation) {
				Cell->setCurrent();
			}
		}
		if (ShowPathAnimation) {
			Acurrent->setFront();
		}
		if (Acurrent->i == goal->i && Acurrent->j == goal->j) {
			found = true;
			return;
		}
		auto it = std::find(OpenSet.begin(), OpenSet.end(), Acurrent);
		OpenSet.erase(it);
		std::vector<cell*> neighbours;
		getNeighbourForPathFinding(Acurrent, wCells, hCells, neighbours, cells);
		for (cell* neighbour : neighbours) {
			float t_gScore = gScore[Acurrent] + dist(Acurrent->i, Acurrent->j, neighbour->i, neighbour->j);
			if (t_gScore < gScore[neighbour]) {
				CameFrom.insert({ neighbour, Acurrent });
				gScore[neighbour] = t_gScore;
				fScore[neighbour] = gScore[neighbour] + hScore[neighbour];
				if (std::find(OpenSet.begin(), OpenSet.end(), neighbour) == OpenSet.end()) {
					OpenSet.push_back(neighbour);
				}
			}
		}
	}
}