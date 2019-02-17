#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char const *argv[])
{
	ofstream imagen;

	imagen.open("../res/imagen.ppm");

	// Cabeceras del archivo
	imagen << "P3" << endl;

	imagen << 1020 << " " << 255 << endl;
	imagen << "256" << endl;


	for (int i = 0; i < 255; i++)
	{
	for (int j = 0; j < 255; j++)
	{
		int color = 255 - i;
		imagen << color << " " << 0 << " " << 0 << endl;
	}

	for (int j = 0; j < (width / 4); j++)
	{
		int color = 255 - i;
		imagen << 0 << " " << color << " " << 0 << endl;
	}
	for (int j = 0; j < (width / 4); j++)
	{
		int color = 255 - i;
		imagen << 0 << " " << 0 << " " << color << endl;
	}

	for (int j = 0; j < (width / 4); j++)
	{
		int color = i;
		imagen << color << " " << color << " " << color << endl;
	}


	}

	imagen.close();
	
	return 0;
}