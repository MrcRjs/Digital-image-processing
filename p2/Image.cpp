#include "Image.h"

/*
	Private helpers
*/

// Func returns true if line is a comment
bool Image::lineIsComment(string l){ return l[0] == '#'; }

void Image::printComments(string * fromLine)
{
	cout << endl << "Comentario: " << endl;

	//string line = fromLine;
	// while there's a line left in file and line is a comment, print comment without # character
	while (!ppmImage.eof() && lineIsComment(*fromLine))
	{
		string l;
		cout << fromLine->substr(2, fromLine->length()) << endl;
		getline(ppmImage, l);
		*fromLine = l;
	}

	cout << endl;
}

string Image::getNextLine(void) {
	// Function to get next non-comment line
	string currentLine;
	getline(ppmImage, currentLine);

	// If line is comment print all consecutive comments
	// and set currentLine to non-comment line
	if (lineIsComment(currentLine))
	{
		printComments(&currentLine);
	}

	return currentLine;

}

void Image::populateMatrix()
{
	// Populate function is a bit different for each format
	if (magic == "P3")
	{
		/* Population algorithm
			for each pixel in file:
				for each channel in pixel:
					push next Integer in file to current pixel
				push pixel to matrix
		*/


		// current chanel variable used to know what color of each pixel we are processing
		// r:0 g:1 b:2
		unsigned short int curChan = 0;
		
		vector <unsigned short int> pixel;
		// While image data in file
		while(!ppmImage.eof())
		{
			// Get next non-comment line
			string dataLine = getNextLine();

			// Stringstream used to get next int https://stackoverflow.com/a/1321154/3011836
			stringstream stream(dataLine);

			// Color value extracted from file
			unsigned short int cVal;

			// Temporary pixel used to store each value for rgb

			// while there's a color value integer in line
			while(stream >> cVal)
			{
				// push color value to pixel
				pixel.push_back(cVal);

				// After setting rgb color values for the pixel
				// pixel is pushed to matrix
				if (pixel.size() == 3)
				{
					matrix.push_back(pixel);
					// pixel is cleared to store next values
					pixel = vector <unsigned short int>();
				}

			}
		}
	}
}

/*
	Public Methods and Funcs
*/

Image::Image(){}

Image::Image(string imgPath)
{
	readImage(imgPath);
}

Image::~Image(){}

void Image::readImage(string file)
{
	ppmImage.open(file);

	// Verify file exists and is not empty
	if (ppmImage.is_open() && !ppmImage.eof())
	{
		
		cout << "Reading file: " << file << endl;
		string line;

		line = getNextLine();

		// Verify file is a valid netpbm image
		if (line == "P1")
		{
			// To-do
			magic = line;
		}
		else if (line ==  "P2")
		{
			// To-do
			magic = line;
		}
		else if (line ==  "P3")
		{
			// Procesing image headers
			magic = line;
			line = getNextLine();

			// To parse height and width we need the position of the space character
			string delim = " ";
			int delimIdx = line.find(delim);

			// Stoi parses str interpreting its content as an integral number of the specified base, which is returned as an int value.
			//  width = the first number from index 0 to space character index
			width = stoi(line.substr(0, delimIdx), nullptr, 0);
			// height = the second number from space character index to line end
			height = stoi(line.substr(delimIdx, line.length()), nullptr, 0);

			line = getNextLine();
			colorDepth = stoi(line);

			// After processing headers we populate the image matrix data
			populateMatrix();
		}
		else {
			cout << "Invalid image format." << endl;
			// Throw Err
		}

		ppmImage.close();
	}
	else {
		cout << "Error reading image file: " << file << endl;
		// Throw err
	}
}


vector <vector <unsigned short int>> Image::getInvertedColorMatrix(void)
{
	// Return inverted colors matrix

	/* Invert Algorithm
		For each pixel p in matrix:
			For each channel c in pixel:
				invertedColor = colorDepth - currentPixelChannelValue
				push invertedColor to invertedPixel
			push invertedPixel to invertedMatrix
	*/
	int matrixSize = matrix.size();
	int channels = matrix[0].size(); 

	vector <vector<unsigned short int>> invDataMatrix;
	
	// For every pixel p in image matrix
	for (int p = 0; p < matrixSize; p++)
	{
		vector <unsigned short int> invPixel;
		
		// For every channel c in each pixel
		for (int c = 0; c < channels; c++)
		{
			// Process inverted value
			// COLOR DEPTH SHOULD BE MAX POSIBLE VALUE FOR EACH VALUE
			// https://en.wikipedia.org/wiki/Netpbm_format
			unsigned short int newCol = colorDepth - matrix[p][c];

			// Push inverted color to inverted pixel
			invPixel.push_back(newCol);
		}
		// Push inverted pixel to inverted matrix
		invDataMatrix.push_back(invPixel);
	}

	return invDataMatrix;
}

vector <vector <unsigned short int>> Image::getBinaryMatrix(int limit)
{
	/* Binary Algorithm
		For each pixel p in matrix:
			For each channel c in pixel:
				binaryColor = if c is >=  than treshold, c = 1 else = 0 
				push binaryColor to binaryPixel
			push binaryPixel to binaryMatrix
	*/
	auto grayMatrix = getGrayscaleMatrix("perceptual");
	int matrixSize = grayMatrix.size();
	int channels = grayMatrix[0].size(); 

	vector <vector<unsigned short int>> binaryMatrix;
	
	// For every pixel p in image grayMatrix
	for (int p = 0; p < matrixSize; p++)
	{
		vector <unsigned short int> binaryPixel;
		
		// For every channel c in each pixel
		unsigned short int newCol = grayMatrix[p][0] >= limit ? colorDepth : 0;

		// Push binary color to inverted pixel
		for (int i = 0; i < 3; i++)
		{
			binaryPixel.push_back(newCol);
		}

		// Push binary pixel to inverted matrix
		binaryMatrix.push_back(binaryPixel);
	}

	return binaryMatrix;
}

vector <vector <unsigned short int>> Image::getFilteredMatrix(void)
{
	auto grayMat = getGrayscaleMatrix("perceptual");

	int matrixSize = grayMat.size(); 

	vector <vector<unsigned short int>> filteredMatrix;
	
	// For every pixel p in image matrix
	for (int p = 0; p < matrixSize; p++)
	{
		vector <unsigned short int> pixel;

		//pixel.push_back(0);
		//pixel.push_back(0);
		pixel.push_back(grayMat[p][0]);
		//pixel.push_back(grayMat[p][0]);
		pixel.push_back(0);
		pixel.push_back(0);
		filteredMatrix.push_back(pixel);
	}
	return filteredMatrix;
};

vector <vector <unsigned short int>> Image::getGrayscaleMatrix(string type = "perceptual")
{
	/* Grayscale Algorithm
		For each pixel p in matrix:
			grayscalePixel = 0.2126 * r + 0.7512 * g + 0.0722 * b
			push grayscalePixel to grayscaleMatrix
	*/

	int matrixSize = matrix.size();
	int channels = matrix[0].size(); 

	vector <vector<unsigned short int>> grayScaleMatrix;
	
	// For every pixel p in image matrix
	for (int p = 0; p < matrixSize; p++)
	{
		vector <unsigned short int> grayscalePixel;
		
		unsigned short int r = matrix[p][0];
		unsigned short int g = matrix[p][1];
		unsigned short int b = matrix[p][2];

		unsigned short int grayPixelColor;

		if (type == "perceptual")
		{
			// Colorimetric (perceptual luminance-preserving)
			// https://en.wikipedia.org/wiki/Grayscale#Converting_color_to_grayscale
			grayPixelColor = (0.2126 * r) + (0.7152 * g) + (0.0722 * b);
		} else if (type == "luma"){
			// Luma
			grayPixelColor = (0.299 * r) + (0.587 * g) + (0.114 * b);
		} else {
			// Use perceptual as
			grayPixelColor = (0.2126 * r) + (0.7152 * g) + (0.0722 * b);
		}

		// For every channel c in each pixel
		for (int c = 0; c < channels; c++)
		{
			grayscalePixel.push_back(grayPixelColor);
		}
		// Push binary pixel to inverted matrix
		grayScaleMatrix.push_back(grayscalePixel);
	}

	return grayScaleMatrix;
}

void Image::printImageToFile(string path = "", vector <vector <unsigned short int>> img =  vector <vector <unsigned short int>>())
{
	// Verify valid path is provided
	if (path != "")
	{
		// TO-DO set default img argument to Image::matrix
		// if (img.size() == 0) { img = getDataMatrix(); }
		ofstream newImg;
		newImg.open(path);

		if (magic == "P3")
		{
			// Write image headers
			newImg << "P3" << endl;
			newImg << width << " " << height << endl;
			newImg << colorDepth << endl;
			
			int matrixSize = img.size();
			int channels = img[0].size();

			for (int p = 0; p < matrixSize; ++p)
			{
				// Write each channel value separated by space char, pixel per line
				for (int c = 0; c < channels; ++c)
				{
					newImg << img[p][c] << " ";
				}
				newImg << endl;
			}
		}

		newImg.close();
	}
	else 
	{
		cout << "Error invalid path provided" << endl;
	}

}

int Image::getColorDepth(void) { return colorDepth; }
int Image::getHeight(void) { return height; }
int Image::getWidth(void) { return width; }
vector <vector <unsigned short int>> Image::getDataMatrix(void) { return matrix; }