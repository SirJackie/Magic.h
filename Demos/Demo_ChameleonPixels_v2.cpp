#include <stdio.h>
#include <math.h>
#include "Magic.h"

#define STRIP_WIDTH 150

int main() {

	Magic();

	Picture picture;
	picture.Load(".\\BMP\\02.bmp");

	int delta = -STRIP_WIDTH;

	for (int i = 0; i < 3000; i++) {

		Clean(0, 0, 0);

		// Draw a BMP Image.
		//picture.Draw(0, 0);

		// Draw a BMP Image Use GetRGB() and SetPixel()
		for (int y = 0; y < picture.height; y++) {
			for (int x = 0; x < picture.width; x++) {
				bool stripped = x + y > delta && x + y < delta + STRIP_WIDTH;

				if (stripped) {
					MagicSetPixel(
						x + 125,
						y + 25,
						x * 255 / picture.width,
						0,
						y * 255 / picture.height
					);
				}

				else {
					MagicSetPixel(
						x + 125,
						y + 25,
						picture.GetR(x, y),
						picture.GetG(x, y),
						picture.GetB(x, y)
					);
				}

			}
		}

		Show();

		delta += 10;
		if (delta >= picture.width * 2) {
			delta = -STRIP_WIDTH;
		}
	}

	Quit();
}
