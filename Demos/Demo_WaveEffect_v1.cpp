#include <stdio.h>
#include <math.h>
#include "Magic.h"

int main() {

	Magic();

	Picture picture;
	picture.Load(".\\BMP\\02.bmp");

	int deltaX = 0;
	float factor = 1.0f;

	for (int i = 0; i < 3000; i++) {

		Clean(202, 219, 237);

		// Draw a BMP Image.
		//picture.Draw(0, 0);

		// Draw a BMP Image Use GetRGB() and SetPixel()
		for (int y = 0; y < picture.height; y++) {
			for (int x = 0; x < picture.width; x++) {
				MagicSetPixel(
					x + 125 + sin((y + deltaX) / 30.0f) * 10,
					y + 25  + sin((x + deltaX) / 30.0f) * 15,
					picture.GetR(x, y),
					picture.GetG(x, y),
					picture.GetB(x, y)
				);
			}
		}

		Show();

		deltaX += 2;
		factor = factor * 0.99f;
	}

	Quit();
}
