#ifndef IMAGE_H
#define IMAGE_H
#include <iostream>
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
		void printImageToFile(string, const vector <vector <unsigned short int>>);

	private:
		fstream ppmImage;
		string magic = "P?";
		int height = 0;
		int width = 0;
		int colorDepth = 0;
		// unsigned short integer for up to 65536 colors  
		vector <vector <unsigned short int>> matrix;
		bool lineIsComment(string);
		void printComments(string *);
		string getNextLine(void);
		void populateMatrix(void);
};

#endif