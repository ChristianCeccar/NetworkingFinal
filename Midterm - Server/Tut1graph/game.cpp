#include "Game.h"
#include <iostream>
#include <GL\glut.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#pragma comment(lib, "Ws2_32.lib")

//Calvin Nunn - 100585602
//Christian Ceccarelli - 100621707

SOCKET server_socket;
struct addrinfo* ptr = NULL;
#define PORT "9898"
#define BUFLEN 512
#define UPDATE_INTERVAL 0.100 //seconds

bool initNetwork() {
	//Initialize winsock
	WSADATA wsa;

	int error;
	error = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (error != 0) {
		printf("Failed to initialize %d\n", error);
		return 0;
	}

	//Create a server socket

	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, PORT, &hints, &ptr) != 0) {
		printf("Getaddrinfo failed!! %d\n", WSAGetLastError());
		WSACleanup();
		return 0;
	}


	server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (server_socket == INVALID_SOCKET) {
		printf("Failed creating a socket %d\n", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	// Bind socket

	if (bind(server_socket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) {
		printf("Bind failed: %d\n", WSAGetLastError());
		closesocket(server_socket);
		freeaddrinfo(ptr);
		WSACleanup();
		return 1;
	}

	/// Change to non-blocking mode
	u_long mode = 1;// 0 for blocking mode
	ioctlsocket(server_socket, FIONBIO, &mode);

	printf("Server is ready!\n");

	return 1;
}



game::game()
{

	initNetwork();
	Paddle.construct("paddle", "meshes/paddle.obj");

	ball.construct("ball", "meshes/ball.obj");

	wall1.construct("wall1", "meshes/paddle.obj");

	wall2.construct("wall2", "meshes/paddle.obj");

	wall3.construct("wall3", "meshes/paddle.obj");

	wall4.construct("wall4", "meshes/paddle.obj");

	wall5.construct("wall5", "meshes/paddle.obj");

	wall6.construct("wall6", "meshes/paddle.obj");

	wall7.construct("wall7", "meshes/paddle.obj");

	wall8.construct("wall8", "meshes/paddle.obj");

	wall9.construct("wall9", "meshes/paddle.obj");

	wall10.construct("wall10", "meshes/paddle.obj");

	wall11.construct("wall11", "meshes/paddle.obj");

	wall12.construct("wall12", "meshes/paddle.obj");

	background.construct("back", "meshes/background.obj");


};

game::~game()
{
	delete updateTimer;

	passThrough.unload();
	PhongNoTexture.unload();
	Paddle.unload();
	ball.unload();
	monkey.unload();
	Phong.unload();
};

void game::initializeGame() {
	updateTimer = new timer();
	glEnable(GL_DEPTH_TEST);
	Paddle.setLocalPosition(glm::vec3(0.f, -5.0f, 0.f));
	ball.setLocalPosition(glm::vec3(-2.f, 0.0f, 0.f));
	//first row
	wall1.setLocalPosition(glm::vec3(-7.5f, 3.f, 0.f));
	wall2.setLocalPosition(glm::vec3(-4.5f, 3.f, 0.f));
	wall3.setLocalPosition(glm::vec3(-1.5f, 3.f, 0.f));
	wall4.setLocalPosition(glm::vec3(1.5f, 3.f, 0.f));
	wall5.setLocalPosition(glm::vec3(4.5f, 3.f, 0.f));
	wall6.setLocalPosition(glm::vec3(7.5f, 3.f, 0.f));
	//second row
	wall7.setLocalPosition(glm::vec3(-7.5f, 4.f, 0.f));
	wall8.setLocalPosition(glm::vec3(-4.5f, 4.f, 0.f));
	wall9.setLocalPosition(glm::vec3(-1.5f, 4.f, 0.f));
	wall10.setLocalPosition(glm::vec3(1.5f, 4.f, 0.f));
	wall11.setLocalPosition(glm::vec3(4.5f, 4.f, 0.f));
	wall12.setLocalPosition(glm::vec3(7.5f, 4.f, 0.f));


	if (!passThrough.Load("shaders/passthrough.vert", "shaders/passthrough.frag"))
	{
		std::cout << "Shaders failed to initialize." << std::endl;
		system("pause");
		exit(0);
	}
	//if (!PhongNoTexture.Load("shaders/PhongNoTexture.vert", "shaders/PhongNoTexture.frag"))
	//{
	//	std::cout << "Shaders failed to initialize." << std::endl;
	//	system("pause");
	//	exit(0);
	//}

	if (!Phong.Load("shaders/Phong.vert", "shaders/Phong.frag"))
	{
		std::cout << "Shaders failed to initialize." << std::endl;
		system("pause");
		exit(0);
	}
	if (!PhongColourSides.Load("shaders/PhongColourSides.vert", "shaders/PhongColourSides.frag"))
	{
		std::cout << "Shaders failed to initialize." << std::endl;
		system("pause");
		exit(0);
	}

	Light Light1, Light2, Light3;

	Light1.PositionorDirection = glm::vec4(0.f, 4.f, 0.f, 1.f);
	Light1.originalPosition = Light1.PositionorDirection;
	Light1.Ambient = glm::vec3(0.f, 0.1f, 0.f); // colour
	Light1.Diffuse = glm::vec3(0.f, 0.7f, 0.f); // colour
	Light1.Specular = glm::vec3(0.f, 1.f, 0.f); // colour
	Light1.SpecularExponent = 50.f;
	Light1.constantAttenuation = 1.f;
	Light1.linearAttenuation = 0.1f;
	Light1.quadraticAttenuation = 0.01f;

	Light2.PositionorDirection = glm::vec4(0.f, -4.f, 0.f, 1.f);
	Light2.originalPosition = Light2.PositionorDirection;
	Light2.Ambient = glm::vec3(0.1f, 0.f, 0.f); // colour
	Light2.Diffuse = glm::vec3(0.7f, 0.f, 0.f); // colour
	Light2.Specular = glm::vec3(1.f, 0.f, 0.f); // colour
	Light2.SpecularExponent = 50.f;
	Light2.constantAttenuation = 1.f;
	Light2.linearAttenuation = 0.1f;
	Light2.quadraticAttenuation = 0.01f;

	Light3.PositionorDirection = glm::vec4(-4.f, 0.f, 0.f, 1.f);
	Light3.originalPosition = Light3.PositionorDirection;
	Light3.Ambient = glm::vec3(0.f, 0.f, 0.1f); // colour
	Light3.Diffuse = glm::vec3(0.f, 0.f, 0.7f); // colour
	Light3.Specular = glm::vec3(0.f, 0.f, 1.f); // colour
	Light3.SpecularExponent = 50.f;
	Light3.constantAttenuation = 1.f;
	Light3.linearAttenuation = 0.1f;
	Light3.quadraticAttenuation = 0.01f;

	pointLights.push_back(Light1);
	pointLights.push_back(Light2);
	pointLights.push_back(Light3);

	directionalLight.PositionorDirection = glm::vec4(-1.f, -1.f, -1.f, 0.f);
	directionalLight.Ambient = glm::vec3(0.05f);
	directionalLight.Diffuse = glm::vec3(0.7f);
	directionalLight.Specular = glm::vec3(1.f);
	directionalLight.SpecularExponent = 50.f;


	if (!monkey.loadFromFile("meshes/monkey.obj"))
	{
		std::cout << "Model failed to load" << std::endl;
		system("pause");
		exit(0);
	}



	Paddle.mat.loadTexture(TextureType::Diffuse, "textures/fur.png");
	Paddle.mat.loadTexture(TextureType::Specular, "Materials/fullSpecular.png");

	ball.mat.loadTexture(TextureType::Diffuse, "textures/normal.jpg");
	ball.mat.loadTexture(TextureType::Specular, "Materials/fullSpecular.png");

	wall1.mat.loadTexture(TextureType::Diffuse, "textures/fur.png");
	wall1.mat.loadTexture(TextureType::Specular, "Materials/fullSpecular.png");

	wall2.mat.loadTexture(TextureType::Diffuse, "textures/fur.png");
	wall2.mat.loadTexture(TextureType::Specular, "Materials/fullSpecular.png");

	wall3.mat.loadTexture(TextureType::Diffuse, "textures/fur.png");
	wall3.mat.loadTexture(TextureType::Specular, "Materials/fullSpecular.png");


	wall4.mat.loadTexture(TextureType::Diffuse, "textures/fur.png");
	wall4.mat.loadTexture(TextureType::Specular, "Materials/fullSpecular.png");

	wall5.mat.loadTexture(TextureType::Diffuse, "textures/fur.png");
	wall5.mat.loadTexture(TextureType::Specular, "Materials/fullSpecular.png");

	wall6.mat.loadTexture(TextureType::Diffuse, "textures/fur.png");
	wall6.mat.loadTexture(TextureType::Specular, "Materials/fullSpecular.png");




	wall7.mat.loadTexture(TextureType::Diffuse, "textures/fur.png");
	wall7.mat.loadTexture(TextureType::Specular, "Materials/fullSpecular.png");

	wall8.mat.loadTexture(TextureType::Diffuse, "textures/fur.png");
	wall8.mat.loadTexture(TextureType::Specular, "Materials/fullSpecular.png");


	wall9.mat.loadTexture(TextureType::Diffuse, "textures/fur.png");
	wall9.mat.loadTexture(TextureType::Specular, "Materials/fullSpecular.png");

	wall10.mat.loadTexture(TextureType::Diffuse, "textures/fur.png");
	wall10.mat.loadTexture(TextureType::Specular, "Materials/fullSpecular.png");

	wall11.mat.loadTexture(TextureType::Diffuse, "textures/fur.png");
	wall11.mat.loadTexture(TextureType::Specular, "Materials/fullSpecular.png");

	wall12.mat.loadTexture(TextureType::Diffuse, "textures/fur.png");
	wall12.mat.loadTexture(TextureType::Specular, "Materials/fullSpecular.png");

	background.mat.loadTexture(TextureType::Diffuse, "textures/normal.jpg");
	background.mat.loadTexture(TextureType::Specular, "Materials/fullSpecular.png");


	cameraTransform = glm::translate(cameraTransform, glm::vec3(0.f, 0.f, -10.f));
	cameraProjection = glm::perspective(45.f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 10000.f);
	//
	//paddleTransform = glm::translate(paddleTransform, glm::vec3(0.f, -5.0f, 0.f));
	//ballTransform = glm::translate(ballTransform, glm::vec3(-2.f, 0.0f, 0.f));


};

bool UpOrDown = true;
bool LeftorRight = true;
int count = 0;
int ballCount = 0;
int brickCount = 0;

std::string player1;
int spectate = 1;

glm::vec3 ballLocation1 = { 0,0,0 };
glm::vec3 ballLocation2 = { 0,0,0 };

glm::vec3 paddleLocation1 = { 0,0,0 };
glm::vec3 paddleLocation2 = { 0,0,0 };

glm::vec3 ballVelocity1 = { 0,0,0 };
glm::vec3 ballVelocity2 = { 0,0,0 };

glm::vec3 paddleVelocity1 = { 0,0,0 };
glm::vec3 paddleVelocity2 = { 0,0,0 };

int player1Score = 0;
int player2Score = 0;
std::vector<std::string> usernames;
int countNames = 0;
void game::update() {
	updateTimer->tick();

	char buf[BUFLEN];
	struct sockaddr_in fromAdder;
	int fromLen;
	fromLen = sizeof(fromAdder);

	memset(buf, 0, BUFLEN);

	int bytes_received = -1;
	int sError = -1;

	bytes_received = recvfrom(server_socket, buf, BUFLEN, 0, (struct sockaddr*) & fromAdder, &fromLen);

	sError = WSAGetLastError();

	if (sError != WSAEWOULDBLOCK && bytes_received > 0)
	{
		char* ipString = inet_ntoa(fromAdder.sin_addr);
		std::string temp = buf;

		std::string sub0	= temp.substr(0					, temp.find("$")					 );
		std::string sub05	= temp.substr(temp.find("$") + 1, temp.find("=") - temp.find("$") - 1);
		std::string sub1	= temp.substr(temp.find("=") + 1, temp.find("_") - temp.find("=") - 1);
		std::string sub2	= temp.substr(temp.find("_") + 1, temp.find("(") - temp.find("_") - 1);
		std::string sub3	= temp.substr(temp.find("(") + 1, temp.find("/") - temp.find("(") - 1);
		std::string sub4	= temp.substr(temp.find("/") + 1, temp.find(" ") - temp.find("/") - 1);
		std::string sub5	= temp.substr(temp.find(" ") + 1, temp.find("]") - temp.find(" ") - 1);
		std::string sub6	= temp.substr(temp.find("]") + 1									 );

		if (usernames.size() <= 0) {
			usernames.push_back(sub0);
		}
		else {
			int newCount = 0;
			for (int i = 0; i < usernames.size(); i++) {
				if (std::strcmp(sub0.c_str(), usernames[i].c_str()) > 0) {
					newCount++;
				}
			}
			if (newCount == usernames.size()) {
				usernames.push_back(sub0);
			}
		}

		if (player1 == "") {
			player1 = sub0;
		}

		std::size_t pos = sub1.find('@');
		sub1 = sub1.substr(0, pos - 1);
		if (sub0 == player1) {
			ballLocation1.x = std::stof(sub1);
			ballVelocity1.x = std::stof(sub4);
		}
		else {
			ballLocation2.x = std::stof(sub1);
			ballVelocity2.x = std::stof(sub4);
		}

		sub1 = temp.substr(temp.find("=") + 1, temp.find("_") - temp.find("="));
		sub1 = sub1.substr(pos + 1);
		if (sub0 == player1) {
			ballLocation1.y = std::stof(sub1);
			ballVelocity1.y = std::stof(sub5);
		}
		else {
			ballLocation2.y = std::stof(sub1);
			ballVelocity2.y = std::stof(sub5);
		}

		pos = sub2.find('(');
		sub2 = sub2.substr(0, pos - 1);
		if (sub0 == player1) {
			paddleLocation1.x = std::stof(sub2);
			paddleVelocity1.x = std::stof(sub6);
		}
		else {
			paddleLocation2.x = std::stof(sub2);
			paddleVelocity2.x = std::stof(sub6);
		}
		sub2 = temp.substr(temp.find("_") + 1, temp.size() - temp.find("_"));
		sub2 = sub2.substr(pos + 1);
		if (sub0 == player1) {
			paddleLocation1.y = std::stof(sub2);
		}
		else {
			paddleLocation2.y = std::stof(sub2);
		}

		if (spectate == 1) {
			ball.setLocalPosition(ballLocation1);
			Paddle.setLocalPosition(paddleLocation1);
			ball.velocity = ballVelocity1;
			Paddle.velocity = paddleVelocity1;
		}
		else {
			ball.setLocalPosition(ballLocation2);
			Paddle.setLocalPosition(paddleLocation2);
			ball.velocity = ballVelocity2;
			Paddle.velocity = paddleVelocity2;
		}

		std::cout << "watching player: " << usernames[spectate - 1] << std::endl;

		if (sub0 == player1) {
			player1Score = std::stoi(sub05);
		}

		else {
			player2Score = std::stoi(sub05);
		}

		if (spectate == 1) {
			std::cout << "score: " << player1Score << std::endl;
		}
		else {
			std::cout << "score: " << player2Score << std::endl;
		}

		std::cout << "balls position: " << ball.localPosition.x << "," << ball.localPosition.y << std::endl;
		std::cout << "paddle position: " << Paddle.localPosition.x << "," << Paddle.localPosition.y << std::endl << std::endl;

		printf("List of Players\n");

		for (int i = 0; i < usernames.size(); i++) {
			printf("%s\n", usernames[i].c_str());
		}
	}
	else{
		if (spectate == 1) {
			Paddle.setLocalPosition(paddleLocation1 + paddleVelocity1);
			ball.setLocalPosition(ballLocation1 + ballVelocity1);
		}
		else {
			Paddle.setLocalPosition(paddleLocation2 + paddleVelocity2);
			ball.setLocalPosition(ballLocation2 + ballVelocity2);
		}
	}
	

	float deltaTime = updateTimer->getElapsedTimeS();

	glm::vec3 currentPaddlePosition = Paddle.getLocalPosition();
	glm::vec3 currentBallPosition = ball.getLocalPosition();
	glm::vec3 tempPosition = currentBallPosition;

	if (ballCount <= 2) {
		if (brickCount <= 12) {
			if (GetAsyncKeyState('1')) {
				spectate = 1;
				
			}

			if (GetAsyncKeyState('2')) {
				spectate = 2;
			}

			
			//else if (GetAsyncKeyState(VK_RIGHT)) {
			//	// = glm::translate(paddleTransform, glm::vec3(0.3f, 0.f, 0.f));
			//	currentPaddlePosition.x += 0.2f;
			//}

			//if (currentPaddlePosition.x >= 8.4) {
			//	currentPaddlePosition.x += -0.2f;
			//}
			//else if (currentPaddlePosition.x <= -8.4) {
			//	currentPaddlePosition.x += 0.2f;
			//}
			//if (currentBallPosition.y <= -8.4) {
			//	currentBallPosition.y = 0.f;
			//	currentBallPosition.x = 0.f;
			//	ballCount++;
			//}
			//
			//if (UpOrDown == false) {
			//	currentBallPosition.y -= 0.04f;
			//}
			//
			//else if (UpOrDown == true && currentBallPosition.y >= 5.2) {
			//	currentBallPosition.y -= 0.1f;
			//	UpOrDown = !UpOrDown;
			//}
			//else {
			//	currentBallPosition.y += 0.04f;
			//}
			//
			//if (ball.collision(Paddle)) {
			//	currentBallPosition.y += 0.1f;
			//	UpOrDown = !UpOrDown;
			//}
			//
			//if (currentBallPosition.x >= 8.4) {
			//	currentBallPosition.x -= 0.1f;
			//	LeftorRight = !LeftorRight;
			//}
			//else if (currentBallPosition.x <= -8.4) {
			//	currentBallPosition.x += 0.1f;
			//	LeftorRight = !LeftorRight;
			//}
			//else if (LeftorRight == true) {
			//	currentBallPosition.x += 0.04f;
			//}
			//else if (LeftorRight == false) {
			//	currentBallPosition.x -= 0.04f;
			//}

			/////////////////////////////////////////////////brick collision
			if (ball.collision(wall1) && UpOrDown == true && wall1Spawn == false) {
				if (count >= 5) {
					wall1Spawn = true;
					brickCount++;

				}
				currentBallPosition.y -= 0.1f;
				UpOrDown = !UpOrDown;
			}

			else if (ball.collision(wall1) && UpOrDown == false && wall1Spawn == false) {
				if (count >= 5) {
					wall1Spawn = true;
					brickCount++;

				}
				currentBallPosition.y += 0.1f;
				UpOrDown = !UpOrDown;
			}

			if (wall1Spawn == true) {
				wall1.setLocalPosition(glm::vec3(1000.f, 0.f, 0.f));
			}
			//////////////////////////////////////////////////////////////////
			if (ball.collision(wall2) && UpOrDown == true && wall2Spawn == false) {
				if (count >= 5) {
					wall2Spawn = true;
					brickCount++;

				}
				currentBallPosition.y -= 0.1f;
				UpOrDown = !UpOrDown;
			}

			else if (ball.collision(wall2) && UpOrDown == false && wall2Spawn == false) {
				if (count >= 5) {
					wall2Spawn = true;
					brickCount++;

				}
				currentBallPosition.y += 0.1f;
				UpOrDown = !UpOrDown;
			}
			if (wall2Spawn == true) {
				wall2.setLocalPosition(glm::vec3(1000.f, 0.f, 0.f));
			}
			/////////////////////////////////////////////////////////////////
			if (ball.collision(wall3) && UpOrDown == true && wall3Spawn == false) {
				if (count >= 5) {
					wall3Spawn = true;
					brickCount++;

				}
				currentBallPosition.y -= 0.1f;
				UpOrDown = !UpOrDown;
			}

			else if (ball.collision(wall3) && UpOrDown == false && wall3Spawn == false) {
				if (count >= 5) {
					wall3Spawn = true;
					brickCount++;

				}
				currentBallPosition.y += 0.1f;
				UpOrDown = !UpOrDown;
			}
			if (wall3Spawn == true) {
				wall3.setLocalPosition(glm::vec3(1000.f, 0.f, 0.f));
			}
			/////////////////////////////////////////////////////////////////
			if (ball.collision(wall4) && UpOrDown == true && wall4Spawn == false) {
				if (count >= 5) {
					wall4Spawn = true;
					brickCount++;

				}
				currentBallPosition.y -= 0.1f;
				UpOrDown = !UpOrDown;
			}

			else if (ball.collision(wall4) && UpOrDown == false && wall4Spawn == false) {
				if (count >= 5) {
					wall4Spawn = true;
					brickCount++;

				}
				currentBallPosition.y += 0.1f;
				UpOrDown = !UpOrDown;
			}
			if (wall4Spawn == true) {
				wall4.setLocalPosition(glm::vec3(1000.f, 0.f, 0.f));
			}
			/////////////////////////////////////////////////////////////////
			if (ball.collision(wall5) && UpOrDown == true && wall5Spawn == false) {
				if (count >= 5) {
					wall5Spawn = true;
					brickCount++;

				}
				currentBallPosition.y -= 0.1f;
				UpOrDown = !UpOrDown;
			}

			else if (ball.collision(wall5) && UpOrDown == false && wall5Spawn == false) {
				if (count >= 5) {
					wall5Spawn = true;
					brickCount++;

				}
				currentBallPosition.y += 0.1f;
				UpOrDown = !UpOrDown;
			}
			if (wall5Spawn == true) {
				wall5.setLocalPosition(glm::vec3(1000.f, 0.f, 0.f));
			}
			/////////////////////////////////////////////////////////////////
			if (ball.collision(wall6) && UpOrDown == true && wall6Spawn == false) {
				if (count >= 5) {
					wall6Spawn = true;
					brickCount++;

				}
				currentBallPosition.y -= 0.1f;
				UpOrDown = !UpOrDown;
			}

			else if (ball.collision(wall6) && UpOrDown == false && wall6Spawn == false) {
				if (count >= 5) {
					wall6Spawn = true;
					brickCount++;

				}
				currentBallPosition.y += 0.1f;
				UpOrDown = !UpOrDown;
			}
			if (wall6Spawn == true) {
				wall6.setLocalPosition(glm::vec3(1000.f, 0.f, 0.f));
			}
			/////////////////////////////////////////////////////////////////
			if (ball.collision(wall7) && UpOrDown == true && wall7Spawn == false) {
				if (count >= 5) {
					wall7Spawn = true;
					brickCount++;

				}
				currentBallPosition.y -= 0.1f;
				UpOrDown = !UpOrDown;
			}

			else if (ball.collision(wall7) && UpOrDown == false && wall7Spawn == false) {
				if (count >= 5) {
					wall7Spawn = true;
					brickCount++;

				}
				currentBallPosition.y += 0.1f;
				UpOrDown = !UpOrDown;
			}
			if (wall7Spawn == true) {
				wall7.setLocalPosition(glm::vec3(1000.f, 0.f, 0.f));
			}
			/////////////////////////////////////////////////////////////////
			if (ball.collision(wall8) && UpOrDown == true && wall8Spawn == false) {
				if (count >= 5) {
					wall8Spawn = true;
					brickCount++;

				}
				currentBallPosition.y -= 0.1f;
				UpOrDown = !UpOrDown;
			}

			else if (ball.collision(wall8) && UpOrDown == false && wall8Spawn == false) {
				if (count >= 5) {
					wall8Spawn = true;
					brickCount++;

				}
				currentBallPosition.y += 0.1f;
				UpOrDown = !UpOrDown;
			}
			if (wall8Spawn == true) {
				wall8.setLocalPosition(glm::vec3(1000.f, 0.f, 0.f));
			}
			/////////////////////////////////////////////////////////////////
			if (ball.collision(wall9) && UpOrDown == true && wall9Spawn == false) {
				if (count >= 5) {
					wall9Spawn = true;
					brickCount++;

				}
				currentBallPosition.y -= 0.1f;
				UpOrDown = !UpOrDown;
			}

			else if (ball.collision(wall9) && UpOrDown == false && wall9Spawn == false) {
				if (count >= 5) {
					wall9Spawn = true;
					brickCount++;

				}
				currentBallPosition.y += 0.1f;
				UpOrDown = !UpOrDown;
			}
			if (wall9Spawn == true) {
				wall9.setLocalPosition(glm::vec3(1000.f, 0.f, 0.f));
			}
			/////////////////////////////////////////////////////////////////
			if (ball.collision(wall10) && UpOrDown == true && wall10Spawn == false) {
				if (count >= 5) {
					wall10Spawn = true;
					brickCount++;

				}
				currentBallPosition.y -= 0.1f;
				UpOrDown = !UpOrDown;
			}

			else if (ball.collision(wall10) && UpOrDown == false && wall10Spawn == false) {
				if (count >= 5) {
					wall10Spawn = true;
					brickCount++;

				}
				currentBallPosition.y += 0.1f;
				UpOrDown = !UpOrDown;
			}
			if (wall10Spawn == true) {
				wall10.setLocalPosition(glm::vec3(1000.f, 0.f, 0.f));
			}
			/////////////////////////////////////////////////////////////////
			if (ball.collision(wall11) && UpOrDown == true && wall11Spawn == false) {
				if (count >= 5) {
					wall11Spawn = true;
					brickCount++;

				}
				currentBallPosition.y -= 0.1f;
				UpOrDown = !UpOrDown;
			}

			else if (ball.collision(wall11) && UpOrDown == false && wall11Spawn == false) {
				if (count >= 5) {
					wall11Spawn = true;
					brickCount++;

				}
				currentBallPosition.y += 0.1f;
				UpOrDown = !UpOrDown;
			}
			if (wall11Spawn == true) {
				wall11.setLocalPosition(glm::vec3(1000.f, 0.f, 0.f));
			}
			/////////////////////////////////////////////////////////////////
			if (ball.collision(wall12) && UpOrDown == true && wall12Spawn == false) {
				if (count >= 5) {
					wall12Spawn = true;
					brickCount++;

				}
				currentBallPosition.y -= 0.1f;
				UpOrDown = !UpOrDown;
			}

			else if (ball.collision(wall12) && UpOrDown == false && wall12Spawn == false) {
				if (count >= 5) {
					wall12Spawn = true;
					brickCount++;

				}
				currentBallPosition.y += 0.1f;
				UpOrDown = !UpOrDown;
			}
			if (wall12Spawn == true) {
				wall12.setLocalPosition(glm::vec3(1000.f, 0.f, 0.f));
			}


		}

	}
	else if (brickCount == 12) {
		currentBallPosition.x = 0.f;
		currentBallPosition.y = 0.f;
	}
	Paddle.setLocalPosition(currentPaddlePosition);
	ball.setLocalPosition(currentBallPosition);
	wall1.update(deltaTime);
	wall2.update(deltaTime);
	wall3.update(deltaTime);
	wall4.update(deltaTime);
	wall5.update(deltaTime);
	wall6.update(deltaTime);
	wall7.update(deltaTime);
	wall8.update(deltaTime);
	wall9.update(deltaTime);
	wall10.update(deltaTime);
	wall11.update(deltaTime);
	wall12.update(deltaTime);
	ball.update(deltaTime);
	Paddle.update(deltaTime);
	if (count <= 121) {
		count++;
	}
};
void game::draw()
{

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Paddle.setShader(Phong, cameraProjection, cameraTransform, pointLights, directionalLight);
	ball.setShader(Phong, cameraProjection, cameraTransform, pointLights, directionalLight);
	wall1.setShader(Phong, cameraProjection, cameraTransform, pointLights, directionalLight);
	wall2.setShader(Phong, cameraProjection, cameraTransform, pointLights, directionalLight);
	wall3.setShader(Phong, cameraProjection, cameraTransform, pointLights, directionalLight);
	wall4.setShader(Phong, cameraProjection, cameraTransform, pointLights, directionalLight);
	wall5.setShader(Phong, cameraProjection, cameraTransform, pointLights, directionalLight);
	wall6.setShader(Phong, cameraProjection, cameraTransform, pointLights, directionalLight);

	wall7.setShader(Phong, cameraProjection, cameraTransform, pointLights, directionalLight);
	wall8.setShader(Phong, cameraProjection, cameraTransform, pointLights, directionalLight);
	wall9.setShader(Phong, cameraProjection, cameraTransform, pointLights, directionalLight);
	wall10.setShader(Phong, cameraProjection, cameraTransform, pointLights, directionalLight);
	wall11.setShader(Phong, cameraProjection, cameraTransform, pointLights, directionalLight);
	wall12.setShader(Phong, cameraProjection, cameraTransform, pointLights, directionalLight);

	glutSwapBuffers();
}

void game::LoadTexture(TextureType Type, const std::string& texFile)
{
	mat.loadTexture(Type, texFile);
}



// input

void game::keyDown(unsigned char key, int mouseX, int mouseY) {

	switch (key) {
	case 27:
	case 'q':
		exit(1);
		break;
	case 't':
		std::cout << "total: "
			<< updateTimer->getCurrentTime() / 1000.0f << std::endl;
	case ' ':
		if (countNames < usernames.size() - 1) {
			countNames++;
		}
		else if (countNames == usernames.size() - 1) {
			countNames = 0;
		}
		
		break;

	case 'l':
		shouldLightSpin = !shouldLightSpin;
		break;
	defaut:
		break;

	}

};
void game::keyUP(unsigned char key, int mouseX, int mouseY) {
	switch (key) {

	default:
		break;

	}

};
void game::mouseClick(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		switch (button)
		{
		case GLUT_LEFT_BUTTON:
			break;
		case GLUT_RIGHT_BUTTON:
			break;
		case GLUT_MIDDLE_BUTTON:
			break;
		default:
			break;
		}

	}


};
void game::mouseMove(int x, int y) {


};