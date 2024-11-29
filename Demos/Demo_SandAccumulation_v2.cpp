#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "Magic.h"

float randFloat() {
	float ZeroToOne = (float)rand() * (1.0f / RAND_MAX);
	return (ZeroToOne - 0.5f) * 2.0f;
}

float sparse(float x, float sparser) {
	return x + randFloat() * sparser;
}

int main() {

	srand((unsigned int)time(NULL));

	Magic();

	Picture picture;
	picture.Load(".\\BMP\\01.bmp");

	float sparser = 500.0f;

	for (int i = 0; i < 3000; i++) {

		Clean(0, 0, 0);

		// Draw a BMP Image.
		//picture.Draw(0, 0);

		// Draw a BMP Image Use GetRGB() and SetPixel()
		for (int y = 0; y < picture.height; y++) {
			for (int x = 0; x < picture.width; x++) {
				MagicSetPixel(
					(int)sparse(x+125, sparser),
					(int)sparse(y+25, sparser),
					picture.GetR(x, y),
					picture.GetG(x, y),
					picture.GetB(x, y)
				);
			}
		}

		Show();
		sparser -= sparser * 0.05f;
		sparser = sparser < 0.0f ? 0.0f : sparser;
	}

	Quit();
}
