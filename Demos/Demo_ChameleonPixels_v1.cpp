#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "Magic.h"

int around(int x, int max) {
	if (x > max / 2) {
		return max - x;
	}
	else {
		return x;
	}
}

int wrap(int x, int max) {
	return x % max;
}

int main() {

	Magic();

	Picture picture;
	picture.Load(".\\BMP\\02.bmp");

	float delta = 0.0f;

	for (int i = 0; i < 3000; i++) {

		Clean(0, 0, 0);

		// Draw a BMP Image.
		//picture.Draw(0, 0);

		// Draw a BMP Image Use GetRGB() and SetPixel()
		/*for (int y = 0; y < picture.height; y++) {
			for (int x = 0; x < picture.width; x++) {
				MagicSetPixel(
					x+125,
					y+25,
					picture.GetR(x, y),
					picture.GetG(x, y),
					picture.GetB(x, y)
				);
			}
		}*/

		// Draw a BMP Image Use GetRGB() and SetPixel()
		for (int y = 0; y < picture.height; y++) {
			for (int x = 0; x < picture.width; x++) {
				MagicSetPixel(
					x + 125,
					y + 25,
					(float)(around(wrap(x + delta, picture.width), picture.width) * 2) / (float)picture.width * 255.0f,
					(float)(around(wrap(y + delta, picture.height), picture.height) * 2) / (float)picture.height * 255.0f,
					0
				);
			}
		}

		Show();

		delta += 1.0f;
	}

	Quit();
}
