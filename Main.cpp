#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iostream>


int main()
{
	std::srand(static_cast<unsigned int>(std::time(NULL)));

	// Define some constants
	const float pi = 3.14159f;
	const int gameWidth = 800;
	const  int gameHeight = 800;
	sf::Vector2f paddleSize(25, 100);
	float ballRadius = 10.f;

	// Create the window of the application
	sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight, 32), "SFML Pong",
		sf::Style::Titlebar | sf::Style::Close);
	window.setVerticalSyncEnabled(true);

	// Load the sounds used in the game
	sf::SoundBuffer ballSoundBuffer;
	if (!ballSoundBuffer.loadFromFile("whatTimeIsIt.flac"))
	{
		return EXIT_FAILURE;
	}
	sf::Sound ballSound(ballSoundBuffer);

	//Create the left paddle
	sf::RectangleShape leftPaddle;
	leftPaddle.setSize(paddleSize - sf::Vector2f(3, 3));
	leftPaddle.setOutlineThickness(1);
	leftPaddle.setOutlineColor(sf::Color::White);
	leftPaddle.setFillColor(sf::Color::White);
	leftPaddle.setOrigin(paddleSize / 2.f);

	//Create the right paddle
	sf::RectangleShape rightPaddle;
	rightPaddle.setSize(paddleSize - sf::Vector2f(3, 3));
	rightPaddle.setOutlineThickness(1);
	rightPaddle.setOutlineColor(sf::Color::White);
	rightPaddle.setFillColor(sf::Color::White);
	rightPaddle.setOrigin(paddleSize / 2.f);

	// Create the ball
	sf::CircleShape ball;
	ball.setRadius(ballRadius - 3);
	ball.setOutlineThickness(3);
	ball.setOutlineColor(sf::Color::Black);
	ball.setFillColor(sf::Color::White);
	ball.setOrigin(ballRadius / 2, ballRadius / 2);

	// Load the text font
	sf::Font font;
	if (!font.loadFromFile("consola.ttf"))
	{
		return EXIT_FAILURE;
	}

	// Initialize the pause message
	sf::Text pauseMessage;
	pauseMessage.setFont(font);
	pauseMessage.setCharacterSize(24);
	pauseMessage.setPosition(170.f, 150.f);
	pauseMessage.setFillColor(sf::Color::White);
	pauseMessage.setString("Press space to start the game \nDesigner: injust");

	// Define the paddle properties
	sf::Clock AllTimer;
	const sf::Time AllTime = sf::seconds(0.1f);
	const float paddleSpeed = 500.f;
	float rightPaddleSpeed = 0.f;
	const float ballSpeed = 500.f;
	float ballAngle = 1.f;

	sf::Clock clock;
	bool isPlaying = false;
	while (window.isOpen())
	{
		// Handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Window closed or escape key pressed: exit
			if ((event.type == sf::Event::Closed) ||
				((event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)))
			{
				window.close();
				break;
			}

			// Space key pressed: play
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space))
			{
				if (!isPlaying)
				{
					// (re)start the game
					isPlaying = true;
					clock.restart();

					// Reset the position of the paddles and ball
					leftPaddle.setPosition(10 + paddleSize.x / 2, gameHeight / 2);
					rightPaddle.setPosition(gameWidth - 10 - paddleSize.x / 2, gameHeight / 2);
					ball.setPosition(gameWidth / 2, gameHeight / 2);

					// Reset the ball angle
					do
					{
						// Make sure the ball initial angle is not too much vertical
						ballAngle = (std::rand() % 360) * 2 * pi / 360;
					} while (std::abs(std::cos(ballAngle)) < 0.7f);
				}
			}
		}

		if (isPlaying)
		{
			float deltaTime = clock.restart().asSeconds();

			// Move the player's paddle
			if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) && (leftPaddle.getPosition().y - paddleSize.y / 2 > 5.f))
			{
				leftPaddle.move(0.f, -paddleSpeed * deltaTime);
			}
			if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) && (leftPaddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f))
			{
				leftPaddle.move(0.f, paddleSpeed * deltaTime);
			}

			// Move the computer's paddle
			if (((rightPaddleSpeed < 0.f) && (rightPaddle.getPosition().y - paddleSize.y / 2 > 5.f)) ||
				((rightPaddleSpeed > 0.f) && (rightPaddle.getPosition().y - paddleSize.y / 2 < gameHeight - 5.f)))
			{
				rightPaddle.move(0.f, rightPaddleSpeed * deltaTime);
			}

			// Update the computer's paddle direction according to the ball position
			if (AllTimer.getElapsedTime() > AllTime)
			{
				AllTimer.restart();
				if (ball.getPosition().y + ballRadius > rightPaddle.getPosition().y + paddleSize.y / 2)
				{
					rightPaddleSpeed = paddleSpeed;
				}
				else if (ball.getPosition().y - ballRadius < rightPaddle.getPosition().y - paddleSize.y / 2)
				{
					rightPaddleSpeed = -paddleSpeed;
				}
				else
				{
					rightPaddleSpeed = 0.f;
				}
			}

			// Move the ball
			float factor = ballSpeed * deltaTime;
			ball.move(std::cos(ballAngle) * factor, std::sin(ballAngle) * factor);

			sf::Vector2f pos = ball.getPosition();
			// TODO writing out the ball position
			std::cout << pos.x << " " << pos.y << std::endl;

			// Check for collisions between the ball and the screen
			if (ball.getPosition().x - ballRadius < 0.f)
			{
				isPlaying = false;
				pauseMessage.setString("You lost!\nPress space to restart or \nescape to exit");
			}
			if (ball.getPosition().x + ballRadius > gameWidth)
			{
				isPlaying = false;
				pauseMessage.setString("You lost!\nPress space to restart or \nescape to exit");
			}
			if (ball.getPosition().y - ballRadius < 0.f)
			{
				ballSound.play();
				ballAngle = -ballAngle;
				ball.setPosition(ball.getPosition().x, ballRadius + 0.1f);
			}
			if (ball.getPosition().y + ballRadius > gameHeight)
			{
				ballSound.play();
				ballAngle = -ballAngle;
				ball.setPosition(ball.getPosition().x, gameHeight - ballRadius - 0.1f);
			}

			// Check the collisions between the ball and the paddles
			// Left paddle
			if (ball.getPosition().x - ballRadius < leftPaddle.getPosition().x + paddleSize.x / 2 &&
				ball.getPosition().x - ballRadius > leftPaddle.getPosition().x &&
				ball.getPosition().y + ballRadius >= leftPaddle.getPosition().y - paddleSize.y / 2 &&
				ball.getPosition().y - ballRadius <= leftPaddle.getPosition().y + paddleSize.y / 2)
			{
				if (ball.getPosition().y > leftPaddle.getPosition().y)
				{
					ballAngle = pi - ballAngle + (std::rand() % 20) * pi / 180;
				}
				else
				{
					ballAngle = pi - ballAngle - (std::rand() % 20) * pi / 180;
				}
				ballSound.play();
				ball.setPosition(leftPaddle.getPosition().x + ballRadius + paddleSize.x / 2 + 0.1f, ball.getPosition().y);
			}

			// Right paddle
			if (ball.getPosition().x + ballRadius > rightPaddle.getPosition().x - paddleSize.x / 2 &&
				ball.getPosition().x + ballRadius < rightPaddle.getPosition().x &&
				ball.getPosition().y + ballRadius >= rightPaddle.getPosition().y - paddleSize.y / 2 &&
				ball.getPosition().y - ballRadius <= rightPaddle.getPosition().y + paddleSize.y / 2)
			{
				if (ball.getPosition().y > rightPaddle.getPosition().y)
				{
					ballAngle = pi - ballAngle + (std::rand() % 20) * pi / 180;
				}
				else
				{
					ballAngle = pi - ballAngle - (std::rand() % 20) * pi / 180;
				}
				ballSound.play();
				ball.setPosition(rightPaddle.getPosition().x - ballRadius - paddleSize.x / 2 - 0.1f, ball.getPosition().y);
			}
		}

		// Window background color 
		window.clear(sf::Color(0,0,0));

		if (isPlaying)
		{
			// Draw the paddles and the ball
			window.draw(leftPaddle);
			window.draw(rightPaddle);
			window.draw(ball);
		}
		else
		{
			// Draw the pause message
			window.draw(pauseMessage);
		}

		// Display things on the screen
		window.display();
	}
		return EXIT_SUCCESS;
}
