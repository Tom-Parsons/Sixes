////////////////////////////////////////////////////////////////////////
// OOP Tutorial 3: Simple C++ OO program to simulate a simple Dice Game 
// VERSION 4
////////////////////////////////////////////////////////////////////////

#include "SFML/Graphics.hpp"

//--------include libraries
#include <ctime>	
#include <iostream>	

using namespace std;
using namespace sf;

enum GameState {
	ASKING_FOR_NUMBER,
	DISPLAYING_RESULT
};

namespace Sixes {
	const int MAX_DIGITS_TO_ENTER = 3;
	const int SCREEN_HEIGHT = 800;
	const int SCREEN_WIDTH = 1200;
	const int SCROLL_SPEED = 20;
}

//--------Score class
class Score {
public:
	void initialise();
	int getAmount() const;
	void updateAmount(int);
private:
	int amount_;
};
void Score::initialise() {
	amount_ = 0;
}
int Score::getAmount() const {
	return amount_;
}
//increment when value>0, decrement otherwise
void Score::updateAmount(int value) {
	amount_ += value;
}
//--------end of Score class

struct GameData {
	Score score;
	Text display;
	string displayText;
	string amountToRoll;
	GameState currentState;
	Texture diceTexture;
	vector<Sprite> dice;
};


//---------------------------------------------------------------------------
int main()
{
	void seed();
	int readInNumberOfThrows();
	void renderWindow(RenderWindow &window, GameData &gameData);

	//seed random number generator
	seed();

	//create and initialise a Score variable
	Score score;
	score.initialise();

	GameState currentState = ASKING_FOR_NUMBER;

	RenderWindow window(sf::VideoMode(Sixes::SCREEN_WIDTH, Sixes::SCREEN_HEIGHT), "Sixes");

	Font font;
	if (!font.loadFromFile("assets/fonts/DroidSans.ttf")) {
		std::cout << "Error: Could not load DroidSans.ttf" << endl;
	}

	Texture diceTexture;
	if (!diceTexture.loadFromFile("assets/dice.png")) {
		std::cout << "Error: Could not load dice.png" << endl;
	}

	string displayText = "How many goes do you want?\n Amount: ";
	Text display;
	display.setPosition(30, 30);
	display.setFont(font);
	display.setCharacterSize(32);
	display.setFillColor(sf::Color::White);

	std::vector<sf::Sprite> dice;

	string amountToRoll = "";

	GameData gameData = { score, display, displayText, amountToRoll, currentState, diceTexture, dice };

	renderWindow(window, gameData);

	return EXIT_SUCCESS;
}

void renderWindow(RenderWindow &window, GameData &gameData) {
	void checkKeyPress(RenderWindow &window, const Event &event, GameData &gameData);

	// Start the game loop
	while (window.isOpen())
	{
		// Process events
		Event event;
		while (window.pollEvent(event))
		{
			// Close window: exit
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			else if (event.type == sf::Event::KeyPressed) {
				checkKeyPress(window, event, gameData);
			}
			else if (event.type == sf::Event::MouseWheelMoved)
			{
				if (event.mouseWheel.delta == 1 && gameData.display.getPosition().y < 30
					) {
					gameData.display.setPosition(gameData.display.getPosition().x, gameData.display.getPosition().y + Sixes::SCROLL_SPEED * (gameData.dice.size() / 10));
					for (Sprite &s : gameData.dice) {
						s.setPosition(s.getPosition().x, s.getPosition().y + Sixes::SCROLL_SPEED * (gameData.dice.size() / 10));
					}
				}
				else if (event.mouseWheel.delta == -1 && gameData.display.getPosition().y + gameData.display.getGlobalBounds().height > Sixes::SCREEN_HEIGHT - 30) {
					gameData.display.setPosition(gameData.display.getPosition().x, gameData.display.getPosition().y - Sixes::SCROLL_SPEED * (gameData.dice.size() / 10));
					for (Sprite &s : gameData.dice) {
						s.setPosition(s.getPosition().x, s.getPosition().y - Sixes::SCROLL_SPEED * (gameData.dice.size() / 10));
					}
				}
			}
		}

		gameData.display.setString(gameData.displayText);

		// Clear screen
		window.clear();

		window.draw(gameData.display);
		for (Sprite s : gameData.dice) {
			window.draw(s);
		}

		// Update the window
		window.display();
	}
}

void checkKeyPress(RenderWindow &window, const Event &event, GameData &gameData) {
	string rollDice(int numberOfGoes, GameData &gameData);

	if (gameData.amountToRoll.size() <= Sixes::MAX_DIGITS_TO_ENTER && gameData.currentState == ASKING_FOR_NUMBER) {
		if (event.key.code > 26 && event.key.code <= 35 && gameData.amountToRoll.size() < Sixes::MAX_DIGITS_TO_ENTER) {
			string key = to_string(event.key.code - 26);
			gameData.amountToRoll += key;
			gameData.displayText += key;
		}
		else if (event.key.code == 26 && gameData.amountToRoll.size() < Sixes::MAX_DIGITS_TO_ENTER) {
			if (gameData.amountToRoll.size() > 0) {
				string key = to_string(event.key.code - 26);
				gameData.amountToRoll += key;
				gameData.displayText += key;
			}
		}
		else if (event.key.code == Keyboard::Enter) {
			gameData.displayText += "\n\nThe original score is: " + to_string(gameData.score.getAmount());
			gameData.displayText += "\n" + rollDice(std::stoi(gameData.amountToRoll), gameData);
			gameData.displayText += "\n\nThe final score is: " + to_string(gameData.score.getAmount()) + "\n\nPress 'Enter' to play another round or any other key to quit...";
			gameData.currentState = DISPLAYING_RESULT;
		}
		else if (event.key.code == Keyboard::Backspace && gameData.amountToRoll.size() > 0) {
			gameData.displayText = gameData.displayText.substr(0, gameData.displayText.size() - 1);
			gameData.amountToRoll = gameData.amountToRoll.substr(0, gameData.amountToRoll.size() - 1);
		}
	}
	else if (gameData.currentState == DISPLAYING_RESULT) {
		if (event.key.code == Keyboard::Enter) {
			gameData.displayText = "How many goes do you want?\n Amount: ";
			gameData.display.setPosition(30, 30);
			gameData.amountToRoll = "";
			gameData.dice.clear();
			gameData.currentState = ASKING_FOR_NUMBER;
		}
		else {
			window.close();
		}
	}
}

string rollDice(const int numberOfGoes, GameData &gameData) {
	int getRandomValue(int);

	string output = "";

	//'roll' dice and update score accordingly 
	for (int i(1); i <= numberOfGoes; i++)
	{
		const int dice = getRandomValue(6);

		Sprite diceSprite(gameData.diceTexture, IntRect(64 * (dice - 1), 0, 64, 64));
		diceSprite.setPosition((Sixes::SCREEN_WIDTH / 2) - 32, 132 + i * 76);

		gameData.dice.push_back(diceSprite);

		output = output + "\nIn try #" + to_string(i) + "\tdice value was: " + to_string(dice) + "\n";
		if (dice == 6)
			gameData.score.updateAmount(1);
	}
	return output;
}

//seed the random number generator from current system time
//so that the numbers will be different every time
void seed() {
	srand(static_cast<unsigned>(time(0)));
}
//produce a random number in range [1..max]
int getRandomValue(const int max) {
	return (rand() % max) + 1;
}
