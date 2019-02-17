#include "Image.h"

using namespace std;

int main(int argc, char const *argv[])
{
	// To Compile:
	// g++ -o p2 prac2.cpp Image.cpp
	Image ppmExample;
	ppmExample.readImage("../res/imagen.ppm");
	
	// Simplified using constructor
	// Image ppmExample = new Image("../res/imagen2.ppm");

	// This type of vector is just a matrix
	// It's a vector OF vectors OF three unsigned short ints representing rgb values for P3 files
	// Vector[Vector[0,255,0], Vector[255,0,255], Vector[0,0,0]] 
	// http://www.cplusplus.com/reference/vector/vector/
	vector <vector <unsigned short int >> img = ppmExample.getDataMatrix();

	ppmExample.printImageToFile("../res/inverted.ppm", ppmExample.getInvertedColorMatrix());
	return 0;
}