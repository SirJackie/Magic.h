#include <stdio.h>
#include <math.h>
#include "Magic.h"

int main() {

	Magic();

	Picture saibao, gameOver;
	saibao.Load(".\\BMP\\01-140px.bmp");
	gameOver.Load(".\\BMP\\GameOver.bmp");

	int saiX = 300, saiY = 100;
	int dtX = 5, dtY = -5;
	bool isGameOver = false;

	while (true) {

		// Clear Screen
		Clean(255, 255, 255);

		// Draw Saibao
		saibao.Draw(saiX, saiY);

		// Move Saibao
		saiX += dtX;
		saiY += dtY;

		// Border Bouncing
		if (saiX <= 0 || saiX >= 800 - saibao.width) {
			dtX = -dtX;
		}
		if (saiY <= 0) {
			dtY = -dtY;
		}

		// Game Over: Touching the Bottom
		if (saiY >= 600 - saibao.height) {
			dtX = 0;
			dtY = 0;
			isGameOver = true;
		}

		if (isGameOver == true) {
			gameOver.Draw(
				(800 - gameOver.width) / 2,
				(600 - gameOver.height) / 2
			);
		}

		// Update Screen
		Show();
	}

	Quit();
}
