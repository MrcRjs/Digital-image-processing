#ifndef IMAGE_H
#define IMAGE_H
#include <iostream>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Image
{
	public:
		Image();
		Image(string);
		~Image();
		void readImage(string);
		int getHeight(void);
		int getWidth(void);
		int getColorDepth(void);
		vector <vector <unsigned short int>> getDataMatrix(void);
		vector <vector <unsigned short int>> getInvertedColorMatrix(void);
		vector <vector <unsigned short int>> getBinaryMatrix(int);
		vector <vector <unsigned short int>> getBinaryUmbralMatrix(int, int);
		vector <vector <unsigned short int>> getBinaryUmbralInvertedMatrix(int, int);
		vector <vector <unsigned short int>> getGrayscaleMatrix(string);
		vector <vector <unsigned short int>> getFilteredMatrix(bool, bool, bool);
		vector <unsigned int> getImageHistogram(unsigned short int, vector <vector <unsigned short int>>);
		vector <vector <unsigned short int>> getGammaCorrectedMatrix(float);
		vector <vector <unsigned short int>> getSumMatrix(vector <vector <unsigned short int>>, vector <vector <unsigned short int>>);
		vector <vector <unsigned short int>> getSubstMatrix(vector <vector <unsigned short int>>, vector <vector <unsigned short int>>);
		vector <vector <unsigned short int>> getGammaCorrectedGrayscaleMatrix(float);
		vector <vector <unsigned short int>> getExtensionUmbralMatrix(int lowerLimit, int upperLimit);
		void printImageToFile(string, const vector <vector <unsigned short int>>);
		void printHistogramToFile(string, vector <unsigned int>);

	private:
		fstream ppmImage;
		string magic = "P?";
		int height = 0;
		int width = 0;
		int colorDepth = 0;
		// unsigned short integer for up to 65536 colors  
		vector <vector <unsigned short int>> matrix;
		vector <vector <unsigned short int>> invertedColorMatrix;
		bool lineIsComment(string);
		void printComments(string *);
		string getNextLine(void);
		void populateMatrix(void);
};

#endif