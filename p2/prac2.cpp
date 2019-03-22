#include "Image.h"
#include <string>

using namespace std;

int main(int argc, char const *argv[])
{
	// To Compile:
	// g++ -o p2 prac2.cpp Image.cpp
	Image ppmExample;
	string resourcesDir = "../res/";
	string imageFile = "panda";
	ppmExample.readImage(resourcesDir + imageFile + ".ppm");
	// Simplified using constructor
	// Image ppmExample = new Image("../res/imagen2.ppm");

	// This type of vector is just a matrix
	// It's a vector OF vectors OF three unsigned short ints representing rgb values for P3 files
	// Vector[Vector[0,255,0], Vector[255,0,255], Vector[0,0,0]] 
	// http://www.cplusplus.com/reference/vector/vector/
	ppmExample.printImageToFile(resourcesDir + imageFile + "-inverted.ppm", ppmExample.getInvertedColorMatrix());
	ppmExample.printImageToFile(resourcesDir + imageFile + "-binary.ppm", ppmExample.getBinaryMatrix(100));
	ppmExample.printImageToFile(resourcesDir + imageFile + "-grayscale-perceptual.ppm", ppmExample.getGrayscaleMatrix("perceptual"));
	ppmExample.printImageToFile(resourcesDir + imageFile + "-grayscale-luma.ppm", ppmExample.getGrayscaleMatrix("luma"));
	ppmExample.printImageToFile(resourcesDir + imageFile + "-red-filter.ppm", ppmExample.getFilteredMatrix());
	ppmExample.printHistogramToFile(resourcesDir + imageFile + "-histogram.ppm", ppmExample.getImageHistogram(0, ppmExample.getGrayscaleMatrix("luma")));
	ppmExample.printImageToFile(resourcesDir + imageFile + "-gamma-grayscale.ppm", ppmExample.getGammaCorrectedGrayscaleMatrix(0.25));
	ppmExample.printImageToFile(resourcesDir + imageFile + "-gamma.ppm", ppmExample.getGammaCorrectedMatrix(2));
	ppmExample.printImageToFile(resourcesDir + imageFile + "-binary-umbral.ppm", ppmExample.getBinaryUmbralMatrix(50, 180));
	ppmExample.printImageToFile(resourcesDir + imageFile + "-binary-umbral-inverted.ppm", ppmExample.getBinaryUmbralInvertedMatrix(50, 180));
	ppmExample.printImageToFile(resourcesDir + imageFile + "-summed.ppm", ppmExample.getSumImages(ppmExample.getFilteredMatrix(), ppmExample.getInvertedColorMatrix()));
	ppmExample.printImageToFile(resourcesDir + imageFile + "-summedGray.ppm", ppmExample.getSumImages(ppmExample.getBinaryUmbralMatrix(50,180), ppmExample.getBinaryUmbralInvertedMatrix(50,180)));
	return 0;
}