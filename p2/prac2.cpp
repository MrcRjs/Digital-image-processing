#include "Image.h"
#include <string>

using namespace std;

int main(int argc, char const *argv[])
{
	// To Compile:
	// g++ -o p2 prac2.cpp Image.cpp
	Image ppmExample;
	string resourcesDir = "../res/";
	string imageFile = "brain";
	ppmExample.readImage(resourcesDir + imageFile + ".ppm");
	//ppmExample.matrix[0][1] = 255;
	// Simplified using constructor
	// Image ppmExample = new Image("../res/imagen2.ppm");

	// This type of vector is just a matrix
	// It's a vector OF vectors OF three unsigned short ints representing rgb values for P3 files
	// Vector[Vector[0,255,0], Vector[255,0,255], Vector[0,0,0]] 
	// http://www.cplusplus.com/reference/vector/vector/
	vector <vector <unsigned short int >> img = ppmExample.getDataMatrix();

	ppmExample.printImageToFile(resourcesDir + imageFile + "-inverted.ppm", ppmExample.getInvertedColorMatrix());
	ppmExample.printImageToFile(resourcesDir + imageFile + "-binary.ppm", ppmExample.getBinaryMatrix(100));
	ppmExample.printImageToFile(resourcesDir + imageFile +"-grayscale-perceptual.ppm", ppmExample.getGrayscaleMatrix("perceptual"));
	ppmExample.printImageToFile(resourcesDir + imageFile +"-grayscale-luma.ppm", ppmExample.getGrayscaleMatrix("luma"));
	ppmExample.printImageToFile(resourcesDir + imageFile +"-red-filter.ppm", ppmExample.getFilteredMatrix());

	return 0;
}