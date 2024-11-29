#include <stdio.h>
#include <math.h>
#include "Magic.h"

int main() {

	Magic();

	Picture saibao, gameOver, paddle, background;
	saibao.Load(".\\BMP\\01-140px.bmp");
	gameOver.Load(".\\BMP\\GameOver.bmp");
	paddle.Load(".\\BMP\\Paddle.bmp");
	background.Load(".\\BMP\\Background.bmp");

	int speed = 7;
	int saiX = 300, saiY = 100;
	int dtX = speed, dtY = -speed;
	bool isGameOver = false;
	int paddleHeight = 600 - paddle.height - 20;

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

		// Paddle
		int paddleCenter = mouseX;
		paddle.Draw(mouseX - paddle.width / 2, paddleHeight);

		// Re-Bounce: Touching the Paddle
		if ((saiY + saibao.height) >= paddleHeight) {
			int saibaoCenter = saiX + saibao.width / 2;

			if (
				(saibaoCenter >= paddleCenter - paddle.width / 2)
				&&
				(saibaoCenter <= paddleCenter + paddle.width / 2)
			) {
				dtY = -dtY;
				saiY -= speed * 2;  // Prevent Corner Case
			}
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
