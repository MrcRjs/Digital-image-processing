#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char const *argv[])
{
	ofstream imagen;

	imagen.open("../res/imagen.ppm");

	// Cabeceras del archivo
	imagen << "P3" << endl;

	const int width = 255 * 4;
	const int height = 255;

	imagen << width << " " << height << endl;
	imagen << "256" << endl;

	int minColor = 0;
	int maxColor = 255;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < (width / 4); j++)
		{
			int currentColor = 255 - i;
			// Rojo a negro
			imagen << currentColor << " " << 0 << " " << 0 << endl;
		}

		for (int j = 0; j < (width / 4); j++)
		{
			int currentColor = 255 - i;
			// Verde a negro
			imagen << 0 << " " << currentColor << " " << 0 << endl;
		}
		for (int j = 0; j < (width / 4); j++)
		{
			int currentColor = 255 - i;
			// Azul a negro
			imagen << 0 << " " << 0 << " " << currentColor << endl;
		}

		for (int j = 0; j < (width / 4); j++)
		{
			int currentColor = i;
			// Negro a blanco
			imagen << currentColor << " " << currentColor << " " << currentColor << endl;
		}


	}

	imagen.close();
	
	return 0;
}