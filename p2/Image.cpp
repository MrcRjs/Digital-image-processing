#include "Image.hpp"

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

Image::Image(vector <vector <unsigned short int>> srcMatrix, int h, int w, int d)
{
	matrix = srcMatrix;
	height = h;
	width = w;
	colorDepth = d;
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
			colorDepth = stoi(line) + 1;

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
	if (invertedColorMatrix.size() > 0)
	{
		return invertedColorMatrix;
	}
	else
	{
		const int matrixSize = matrix.size();
		const int channels = matrix[0].size(); 

		vector <vector <unsigned short int>> invDataMatrix(matrixSize, vector <unsigned short int>(channels, 0));
		
		// For every pixel p in image matrix
		for (int p = 0; p < matrixSize; p++)
		{
			// For every channel c in each pixel
			for (int c = 0; c < channels; c++)
			{
				// Process inverted value
				// COLOR DEPTH SHOULD BE MAX POSIBLE VALUE FOR EACH VALUE
				// https://en.wikipedia.org/wiki/Netpbm_format
				unsigned short int newCol = colorDepth - 1 - matrix[p][c];

				// Push inverted color to inverted pixel
				invDataMatrix[p][c] = newCol;
			}
		}

		return invDataMatrix;
	}
}

vector <vector <unsigned short int>> Image::getBinaryMatrix(int treshold)
{
	/* Binary Algorithm
		For each pixel p in matrix:
			For each channel c in pixel:
				binaryColor = if c is >=  than treshold, c = 1 else = 0 
				set binaryColor
	*/
	const auto grayMatrix = getGrayscaleMatrix("perceptual");
	const int matrixSize = grayMatrix.size();
	const unsigned short int channels = 3; 

	vector <vector<unsigned short int>> binaryMatrix(matrixSize, vector <unsigned short int>(channels, 0));
	
	// For every pixel p in image grayMatrix
	for (int p = 0; p < matrixSize; p++)
	{		
		unsigned short int newCol = grayMatrix[p][0] >= treshold ? colorDepth - 1 : 0;

		for (int c = 0; c < channels; c++)
		{
			binaryMatrix[p][c] = newCol;
		}
	}

	return binaryMatrix;
}

vector <vector <unsigned short int>> Image::getBinaryUmbralMatrix(int lowerLimit, int upperLimit)
{
	/* Binary Umbral Algorithm
		For each pixel p in matrix:
			For each channel c in pixel:
				binaryUmbralColor = if c is >=  than lowerLimit AND c <= upperLimit, c = MaxPossibleColor else = 0 
				set binaryUmbralColor
	*/
	const auto grayMatrix = getGrayscaleMatrix("perceptual");
	const int matrixSize = grayMatrix.size();
	const unsigned short int channels = 3;

	vector <vector<unsigned short int>> binaryUmbralMatrix(matrixSize, vector <unsigned short int>(channels, 0));
	
	// For every pixel p in image grayMatrix
	for (int p = 0; p < matrixSize; p++)
	{	
		unsigned short int newCol = grayMatrix[p][0] >= lowerLimit && grayMatrix[p][0] <= upperLimit  ? colorDepth - 1 : 0;

		for (int c = 0; c < channels; c++)
		{
			binaryUmbralMatrix[p][c] = newCol;
		}
	}

	return binaryUmbralMatrix;
}

vector <vector <unsigned short int>> Image::getExtensionUmbralMatrix(int lowerLimit, int upperLimit)
{
	/* Binary Umbral Algorithm
		For each pixel p in matrix:
			For each channel c in pixel:
				binaryUmbralColor = if c is >=  than lowerLimit AND c <= upperLimit, c = MaxPossibleColor else = 0 
				set binaryUmbralColor
	*/
	const auto grayMatrix = getGrayscaleMatrix("perceptual");
	const int matrixSize = grayMatrix.size();
	const unsigned short int channels = 3;

	vector <vector<unsigned short int>> binaryUmbralMatrix(matrixSize, vector <unsigned short int>(channels, 0));
	
	// For every pixel p in image grayMatrix
	for (int p = 0; p < matrixSize; p++)
	{	
		unsigned short int newCol = grayMatrix[p][0] >= lowerLimit && grayMatrix[p][0] <= upperLimit  ? (grayMatrix[p][0] - lowerLimit) * ((colorDepth - 1) / (upperLimit - lowerLimit)) : colorDepth - 1;

		for (int c = 0; c < channels; c++)
		{
			binaryUmbralMatrix[p][c] = newCol;
		}
	}

	return binaryUmbralMatrix;
}

vector <vector <unsigned short int>> Image::getBinaryUmbralInvertedMatrix(int lowerLimit, int upperLimit)
{
	/* Binary Umbral Inverted Algorithm
		For each pixel p in matrix:
			For each channel c in pixel:
				binaryUmbralInvertedColor = if c is >=  than lowerLimit AND c <= upperLimit, c = 0 else = MaxPossibleColor 
				set binaryUmbralInvertedColor
	*/
	const auto grayMatrix = getGrayscaleMatrix("perceptual");
	const int matrixSize = grayMatrix.size();
	const unsigned short int channels = 3; 

	vector <vector<unsigned short int>> binaryUmbralInvertedMatrix(matrixSize, vector <unsigned short int>(channels, 0));
	
	// For every pixel p in image grayMatrix
	for (int p = 0; p < matrixSize; p++)
	{
		unsigned short int newCol = grayMatrix[p][0] >= lowerLimit && grayMatrix[p][0] <= upperLimit  ? 0 : colorDepth - 1;
		for (int c = 0; c < channels; c++)
		{
			binaryUmbralInvertedMatrix[p][c] = newCol;
		}
	}

	return binaryUmbralInvertedMatrix;
}

vector <vector <unsigned short int>> Image::getFilteredMatrix(bool r = 1, bool g = 0, bool b = 0 )
{
	const auto grayMat = getGrayscaleMatrix("perceptual");
	const int matrixSize = grayMat.size();
	const unsigned short int channels = 3;

	vector <vector<unsigned short int>> filteredMatrix(matrixSize, vector <unsigned short int>(channels, 0));;
	
	// For every pixel p in image matrix
	for (int p = 0; p < matrixSize; p++)
	{
		filteredMatrix[p][0] = r ? grayMat[p][0] : 0;
		filteredMatrix[p][1] = g ? grayMat[p][1] : 0;
		filteredMatrix[p][2] = b ? grayMat[p][2] : 0;
	}
	return filteredMatrix;
};

vector <vector <unsigned short int>> Image::getSumMatrix(vector <vector <unsigned short int>> imgA, vector <vector <unsigned short int>> imgB)
{
	const int totalPixels = imgA.size();

	// Construc vector of totalPixels size, each with a vector of size 3 and a value of 0
	// {vector[0]{vector{0,0,0}}, vector[1]{vector{0,0,0}}, ... vector[totalPixels][{or{0,0,0}}}
	vector <vector <unsigned short int>> sumMatrix(totalPixels, vector <unsigned short int> (3, 0));

	// For each pixel set the sum (imgAPixel + imgBPixel) / 2
	for (int i = 0; i < totalPixels; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			sumMatrix[i][j] = int(((imgA[i][j] + imgB[i][j]) / 2));
		}
	}
	return sumMatrix;
}

vector <vector <unsigned short int>> Image::getSumFarsMatrix(vector <vector <unsigned short int>> imgA, vector <vector <unsigned short int>> imgB)
{
	if(imgA.size() == imgB.size()) {

		const int totalPixels = imgA.size();

		// Construc vector of totalPixels size, each with a vector of size 3 and a value of 0
		// {vector[0]{vector{0,0,0}}, vector[1]{vector{0,0,0}}, ... vector[totalPixels][{or{0,0,0}}}
		vector <vector <unsigned short int>> farsSumMatrix(totalPixels, vector <unsigned short int> (3, 0));

		const int maxCol = colorDepth - 1;
		// For each pixel set fars value
		for (int p = 0; p < totalPixels; ++p)
		{
			for (int c = 0; c < 3; ++c)
			{
				int sum = imgA[p][c] + imgB[p][c];

				// If sum < max posible color, set sum value
				// else set max posible color
				farsSumMatrix[p][c] = sum < maxCol ? sum : maxCol;
			}
		}
		return farsSumMatrix;
	}
	else {
		// throw err
		cout << "Error: Images have different dimensions.";
		return vector <vector <unsigned short int>>(0, vector<unsigned short int>(0,0));
	}
}

vector <vector <unsigned short int>> Image::getSubstMatrix(vector <vector <unsigned short int>> imgA, vector <vector <unsigned short int>> imgB)
{
	const int totalPixels = imgA.size();

	// Construc vector of totalPixels size, each with a vector of size 3 and a value of 0
	// {vector[0]{vector{0,0,0}}, vector[1]{vector{0,0,0}}, ... vector[totalPixels][{or{0,0,0}}}
	vector <vector <unsigned short int>> subsMatrix(totalPixels, vector <unsigned short int> (3, 0));

	// For each pixel set substraction Absolute |imgAPixel - imgBPixel|
	for (int i = 0; i < totalPixels; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			subsMatrix[i][j] = fabs((imgA[i][j] - imgB[i][j])*2);
		}
	}
	return subsMatrix;
}

vector <vector <unsigned short int>> Image::getGrayscaleMatrix(string type = "perceptual")
{
	/* Grayscale Algorithm
		For each pixel p in matrix:
			grayscalePixel = 0.2126 * r + 0.7512 * g + 0.0722 * b
			push grayscalePixel to grayScaleMatrix
	*/

	if (type == "perceptual" && grayScalePerceptualMatrix.size() > 0)
	{
		return grayScalePerceptualMatrix;
	}
	else if(type == "luma" && grayScaleLumaMatrix.size() > 0)
	{
		return grayScaleLumaMatrix;
	}
	else
	{
		const int matrixSize = matrix.size();
		const unsigned short int channels = 3; 

		vector <vector<unsigned short int>> grayScaleMatrix(matrixSize, vector <unsigned short int>(channels, 0));;
		
		// For every pixel p in image matrix
		for (int p = 0; p < matrixSize; p++)
		{
			const unsigned short int r = matrix[p][0];
			const unsigned short int g = matrix[p][1];
			const unsigned short int b = matrix[p][2];

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
				grayScaleMatrix[p][c] = grayPixelColor;
			}
		}

		// Save gray matrix to object matrix
		if (type == "luma")
		{
			grayScaleLumaMatrix = grayScaleMatrix;
		}
		else
		{
			grayScalePerceptualMatrix = grayScaleMatrix;
		}

		return grayScaleMatrix;
	}
}

vector <unsigned int> Image::getImageHistogram(unsigned short int c = 0, vector <vector <unsigned short int>> imageMatrix = vector <vector <unsigned short int>>()) {
	// Construct vector with colorDepth elements started in 0
	vector <unsigned int> histVals(colorDepth, 0);

	const int totalPixels = imageMatrix.size();

	// For every pixel in image add 1 to correspondig histogram value
	for (int i = 0; i < totalPixels; ++i)
	{
		histVals[imageMatrix[i][c]]++;
	}

	return histVals;
}

vector <vector <unsigned short int>> Image::getGammaCorrectedGrayscaleMatrix(float gamma)
{
	const auto grayScaleMatrix = getGrayscaleMatrix("luma");
	const int totalPixels = grayScaleMatrix.size();

	// Construc vector of totalPixels size, each with a vector of size 3 and a value of 0
	// {vector[0]{vector{0,0,0}}, vector[1]{vector{0,0,0}}, ... vector[totalPixels][{or{0,0,0}}}
	vector <vector <unsigned short int>> gammaCorrectedMatrix(totalPixels, vector <unsigned short int> (3, 0));


	// For each pixel set the gamma corrected color
	for (int i = 0; i < totalPixels; ++i)
	{
		float gammaCorrectedColor = colorDepth * pow((grayScaleMatrix[i][0] / float(colorDepth)), gamma);
		gammaCorrectedMatrix[i][0] = int(gammaCorrectedColor);
		gammaCorrectedMatrix[i][1] = int(gammaCorrectedColor);
		gammaCorrectedMatrix[i][2] = int(gammaCorrectedColor);
	}
	return gammaCorrectedMatrix;
}

vector <vector <unsigned short int>> Image::getGammaCorrectedMatrix(float gamma)
{
	const auto originalMatrix = matrix;
	const int totalPixels = originalMatrix.size();

	// Construc vector of totalPixels size, each with a vector of size 3 and a value of 0
	// {vector[0]{vector{0,0,0}}, vector[1]{vector{0,0,0}}, ... vector[totalPixels][{or{0,0,0}}}
	vector <vector <unsigned short int>> gammaCorrectedMatrix(totalPixels, vector <unsigned short int> (3, 0));


	// For each pixel set the gamma corrected color
	for (int i = 0; i < totalPixels; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			const float gammaCorrectedColor = colorDepth * pow((originalMatrix[i][j] / float(colorDepth)), gamma);
			gammaCorrectedMatrix[i][j] = int(gammaCorrectedColor);
		}
	}
	return gammaCorrectedMatrix;
}

vector <vector <unsigned short int>> Image::getLogaritmicMatrix(int alpha = 1)
{
	const auto originalMatrix = matrix;
	const int totalPixels = originalMatrix.size();

	// Construc vector of totalPixels size, each with a vector of size 3 and a value of 0
	// {vector[0]{vector{0,0,0}}, vector[1]{vector{0,0,0}}, ... vector[totalPixels][{or{0,0,0}}}
	vector <vector <unsigned short int>> logaritmicMatrix(totalPixels, vector <unsigned short int> (3, 0));

	if (alpha < 1)
	{
		alpha = 1;
	}
	const int A = abs((colorDepth - 1) / float (log(alpha * colorDepth)));

	// For each pixel set the logaritmic color
	for (int i = 0; i < totalPixels; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			logaritmicMatrix[i][j] = A * log((alpha * originalMatrix[i][j]) + 1);
		}
	}
	return logaritmicMatrix;
}

vector <vector <unsigned short int>> Image::getSinMatrix(void)
{
	const auto originalMatrix = matrix;
	const int totalPixels = originalMatrix.size();

	// Construc vector of totalPixels size, each with a vector of size 3 and a value of 0
	// {vector[0]{vector{0,0,0}}, vector[1]{vector{0,0,0}}, ... vector[totalPixels][{or{0,0,0}}}
	vector <vector <unsigned short int>> sinMatrix(totalPixels, vector <unsigned short int> (3, 0));



	// For each pixel set sin color
	for (int i = 0; i < totalPixels; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			if (originalMatrix[i][j] == 255)
			{
				sinMatrix[i][j] = 255;
			}
			else 
			{
				sinMatrix[i][j] = (colorDepth - 1) * sin((M_PI * originalMatrix[i][j]) / (2 * (colorDepth - 1)));
			}
		}
	}
	return sinMatrix;
}

vector <vector <unsigned short int>> Image::getBordersMatrix()
{
	const auto grayScaleMatrix = getGrayscaleMatrix("luma");
	const int totalPixels = grayScaleMatrix.size();
	// Construc vector of totalPixels size, each with a vector of size 3 and a value of 0
	// {vector[0]{vector{0,0,0}}, vector[1]{vector{0,0,0}}, ... vector[totalPixels][{or{0,0,0}}}
	vector <vector <unsigned short int>> bordersMatrix(totalPixels, vector <unsigned short int> (3, 0));


	for (int i = 0; i < height - 1; ++i)
	{
		for (int j = 0; j < width - 1; ++j)
		{
			// Calculate first derivates x and y
			const int resx = grayScaleMatrix[getVectorIndex(i + 1, j)][0] -  grayScaleMatrix[getVectorIndex(i, j)][0];
			const int resy = grayScaleMatrix[getVectorIndex(i, j + 1)][0] -  grayScaleMatrix[getVectorIndex(i, j)][0];

			// Get 
			const int resSum = sqrt((resx * resx) + (resy * resy));
			
			// Assign result to border matrix
			const int curPix = getVectorIndex(i, j);
			for (int k = 0; k < 3; ++k)
			{
				bordersMatrix[curPix][k] = resSum;
			}
		}
	}
	return bordersMatrix;
};

vector <vector <unsigned short int>> Image::getBordersGradMatrix()
{
	const auto grayScaleMatrix = getGrayscaleMatrix("luma");
	const int totalPixels = grayScaleMatrix.size();
	// Construc vector of totalPixels size, each with a vector of size 3 and a value of 0
	// {vector[0]{vector{0,0,0}}, vector[1]{vector{0,0,0}}, ... vector[totalPixels][{or{0,0,0}}}
	vector <vector <unsigned short int>> bordersGradMatrix(totalPixels, vector <unsigned short int> (3, 0));

	// Using Sobel operator
	// https://en.wikipedia.org/wiki/Sobel_operator
	const short int gradx[3][3] = {
		{ -1, 0, 1 },
		{ -2, 0, 2 },
		{ -1, 0, 1 },
	};

	const short int grady[3][3] = { 
		{ -1, -2, -1 },
		{ 0, 0, 0 },
		{ 1, 2, 1 },
	};


	// For each pixel, leaving one pixel border
	// to operate neighbors
	for (int i = 1; i < height - 1; ++i)
	{
		for (int j = 1; j < width - 1; ++j)
		{
			// For each kernel neighbor value
			// First using gradient X to get horizontal borders
			// Seconnd using gradient Y to get verticarl borders
			int resx = 0;
			for (int a = -1; a < 2; ++a)
			{
				for (int b = -1; b < 2; ++b)
				{
					resx += gradx[a + 1][b + 1] * grayScaleMatrix[getVectorIndex(i + a, j + b)][0];
				}
			}

			int resy = 0;
			for (int a = -1; a < 2; ++a)
			{
				for (int b = -1; b < 2; ++b)
				{
					resy += grady[a + 1][b + 1] * grayScaleMatrix[getVectorIndex(i + a, j + b)][0];
				}
			}

			// Compute sum using absolute value
			// multiplye by 1/2 to get light lines
			int resSum = 0.5 * sqrt((resx * resx) + (resy * resy));
			
			// Normalize values between 0 and max possible value 
			if (resSum < 0)
			{
				resSum = 0;
			}
			
			if (resSum > (colorDepth - 1))
			{
				resSum = colorDepth - 1;
			}

			// Set color value to each channel
			const int curPix = getVectorIndex(i, j);
			for (int k = 0; k < 3; ++k)
			{
				bordersGradMatrix[curPix][k] = resSum;
			}
		}
	}
	return bordersGradMatrix;
};

vector <vector <unsigned short int>> Image::getGaussMatrix()
{
	const auto originalMatrix = matrix;
	const int totalPixels = originalMatrix.size();
	// Construc vector of totalPixels size, each with a vector of size 3 and a value of 0
	// {vector[0]{vector{0,0,0}}, vector[1]{vector{0,0,0}}, ... vector[totalPixels][{or{0,0,0}}}
	vector <vector <unsigned short int>> gaussMatrix(totalPixels, vector <unsigned short int> (3, 0));

	// Using kernel Gaussian blur 3x3
	// https://en.wikipedia.org/wiki/Kernel_(image_processing)
	const float kernelGauss[3][3] = {
		{ 1/16.0, 2/16.0, 1/16.0 },
		{ 2/16.0, 4/16.0, 2/16.0 },
		{ 1/16.0, 2/16.0, 1/16.0 },
	};

	// For each pixel, leaving one pixel border
	// to operate neighbors
	for (int i = 1; i < height - 1; ++i)
	{
		for (int j = 1; j < width - 1; ++j)
		{
			// Used to calculate add the resultant blurred pixel values
			vector <unsigned short int> resGauss(3, 0);

			// For each kernel neighbor value
			for (int a = -1; a < 2; ++a)
			{
				for (int b = -1; b < 2; ++b)
				{
					// For each channel
					// Todo: implement n channel support
					for (int c = 0; c < 3; ++c)
					{
						// Sum gauss[a][b] constant value multiplied by neighor[a][b] 
						resGauss[c] += kernelGauss[a + 1][b + 1] * originalMatrix[getVectorIndex(i + a, j + b)][c];
					}
		
				}
			}

			// Set pixel values to resultant matrix image
			const int curPix = getVectorIndex(i, j);
			gaussMatrix[curPix] = resGauss;
		}
	}
	return gaussMatrix;
};

vector <vector <unsigned short int>> Image::getGaussMatrix(vector <vector <unsigned short int>> sourceMatrix)
{
	const int totalPixels = sourceMatrix.size();
	// Construc vector of totalPixels size, each with a vector of size 3 and a value of 0
	// {vector[0]{vector{0,0,0}}, vector[1]{vector{0,0,0}}, ... vector[totalPixels][{or{0,0,0}}}
	vector <vector <unsigned short int>> gaussMatrix(totalPixels, vector <unsigned short int> (3, 0));

	// Using kernel Gaussian blur 3x3
	// https://en.wikipedia.org/wiki/Kernel_(image_processing)
	const float kernelGauss[3][3] = {
		{ 1/16.0, 2/16.0, 1/16.0 },
		{ 2/16.0, 4/16.0, 2/16.0 },
		{ 1/16.0, 2/16.0, 1/16.0 },
	};

	// For each pixel, leaving one pixel border
	// to operate neighbors
	for (int i = 1; i < height - 1; ++i)
	{
		for (int j = 1; j < width - 1; ++j)
		{
			// Used to calculate add the resultant blurred pixel values
			vector <unsigned short int> resGauss(3, 0);

			// For each kernel neighbor value
			for (int a = -1; a < 2; ++a)
			{
				for (int b = -1; b < 2; ++b)
				{
					// For each channel
					// Todo: implement n channel support
					for (int c = 0; c < 3; ++c)
					{
						// Sum gauss[a][b] constant value multiplied by neighor[a][b] 
						resGauss[c] += kernelGauss[a + 1][b + 1] * sourceMatrix[getVectorIndex(i + a, j + b)][c];
					}
		
				}
			}

			// Set pixel values to resultant matrix image
			const int curPix = getVectorIndex(i, j);
			gaussMatrix[curPix] = resGauss;
		}
	}
	return gaussMatrix;
};

vector <vector <unsigned short int>> Image::getMeanMatrix(int n)
{
	auto operationsMatrix = matrix;
	const int totalPixels = operationsMatrix.size();
	// Construc vector of totalPixels size, each with a vector of size 3 and a value of 0
	// {vector[0]{vector{0,0,0}}, vector[1]{vector{0,0,0}}, ... vector[totalPixels][{or{0,0,0}}}
	vector <vector <unsigned short int>> meanMatrix(totalPixels, vector <unsigned short int> (3, 0));

	// Using box blur 3x3 or aritmetic mean
	// https://en.wikipedia.org/wiki/Kernel_(image_processing)
	const float boxBlur = 1/9.0;

	// For each pixel, leaving one pixel border
	// to operate neighbors
	for (int k = 0; k < n; ++k)
	{
		for (int i = 1; i < height - 1; ++i)
		{
			for (int j = 1; j < width - 1; ++j)
			{
				// Used to calculate add the resultant blurred pixel values
				vector <unsigned short int> resBoxBlur(3, 0);

				// For each kernel neighbor value
				for (int a = -1; a < 2; ++a)
				{
					for (int b = -1; b < 2; ++b)
					{
						// For each channel
						// Todo: implement n channel support
						for (int c = 0; c < 3; ++c)
						{
							// Sum box blur 1/9 constant value multiplied by neighor[a][b] 
							resBoxBlur[c] += boxBlur * operationsMatrix[getVectorIndex(i + a, j + b)][c];
						}
			
					}
				}

				// Set pixel values to resultant matrix image
				const int curPix = getVectorIndex(i, j);
				meanMatrix[curPix] = resBoxBlur;
			}
		}
		operationsMatrix = meanMatrix;
	}
	return meanMatrix;
};

vector <vector <unsigned short int>> Image::getMeanGeometricMatrix(int n)
{
	auto operationsMatrix = matrix;
	const int totalPixels = operationsMatrix.size();
	// Construc vector of totalPixels size, each with a vector of size 3 and a value of 0
	// {vector[0]{vector{0,0,0}}, vector[1]{vector{0,0,0}}, ... vector[totalPixels][{or{0,0,0}}}
	vector <vector <unsigned short int>> geoMeanMatrix(totalPixels, vector <unsigned short int> (3, 1));

	// For each pixel, leaving one pixel border
	// to operate neighbors
	for (int k = 0; k < n; ++k)
	{
		for (int i = 1; i < height - 1; ++i)
		{
			for (int j = 1; j < width - 1; ++j)
			{
				// Used to convert float values to resultant unsigned short int values
				vector <unsigned short int> resGeom(3, 1);
				// Used as auxiliar to store float values
				vector <float> resFloat(3, 1.0);

				// For each kernel neighbor value
				for (int a = -1; a < 2; ++a)
				{
					for (int b = -1; b < 2; ++b)
					{
						// For each channel
						// Todo: implement n channel support
						for (int c = 0; c < 3; ++c)
						{
							// geometric value = (X1 * X2 * ... * Xn) ^ (1/n)
							float geomVal = pow(operationsMatrix[getVectorIndex(i + a, j + b)][c], 1/9.0);
							// floating result = 1 if geometric value < 1
							resFloat[c] *= geomVal >= 1 ? geomVal : 1;
						}
			
					}
				}

				// Asign result rounding floating numer for each channel
				for (int c = 0; c < 3; ++c)
				{
					resGeom[c] = round(resFloat[c]);
				}

				// Set pixel values to resultant matrix image
				const int curPix = getVectorIndex(i, j);
				geoMeanMatrix[curPix] = resGeom;
			}
		}
		operationsMatrix = geoMeanMatrix;
	}
	return geoMeanMatrix;
};

int Image::getVectorIndex(int x, int y)
{
	// Returns one-dimension vector[x] index
	// given matrix[x][y] indexes
	// Since Image.matrix is a vector of pixels
	// when we need to do kernel operations
	// it's easy to work with neighbors
	// using [x][y] indexes
	return (width * x) + y;
}

void Image::printHistogramToFile(string path = "", vector <unsigned int> histogramMatrix =  vector <unsigned int>())
{
		// Verify valid path is provided
	if (path != "")
	{
		ofstream histFile(path);
		
		if (histFile)
		{
			// Set Headers
			// Get max value in histogram to set image width
			// https://stackoverflow.com/a/9874912/3011836
			const int maxVal = *max_element(histogramMatrix.begin(), histogramMatrix.end());
			// Note that in P1 format, a 0 signifies a white pixel, and a 1 signifies a black pixel
			histFile << "P1" << endl;

			const int heightScale = maxVal / colorDepth;
			histFile << maxVal << " " << colorDepth * heightScale << endl;
			const int histTotalcolors = histogramMatrix.size();

			for (int i = 0; i < histTotalcolors; ++i)
			 {
			 	for (int j = 0; j < heightScale; ++j)
			 	{
			 		// For current color print n WHITE pixels
				 	const int currentVal = histogramMatrix[i];
				 	for (int i = 0; i < currentVal; ++i)
				 	{
				 		histFile << 0 << " ";
				 	}
				 	// For max histogram value - current histogram value print BLACK pixels
				 	const int whiteDelta = maxVal - currentVal;
				 	for (int i = 0; i < whiteDelta; ++i)
				 	{
				 		histFile << 1 << " ";
				 	}
				 	histFile << endl;
			 	}
			 } 

			histFile.close();	
		}
		else
		{
			// Throw err
			cout << "Error opening file " << path;
		}
	}
	else 
	{
		cout << "Error invalid path provided" << endl;
	}

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
			newImg << colorDepth - 1 << endl;
			
			const int matrixSize = img.size();
			const unsigned short int channels = 3;

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