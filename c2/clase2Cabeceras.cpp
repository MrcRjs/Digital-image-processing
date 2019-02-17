/*
Author: Marco A. Rojas <marco.rojas at alumno.buap.mx>
*/

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

string line;
ifstream ppmImage("../res/imagen2.ppm");

bool lineIsComment(){ return line[0] == '#'; }

void printComments()
{
	cout << endl << "Comentario: " << endl;

	// while line is a comment, print comment without # character
	while (lineIsComment())
	{
		cout << line.substr(2, line.length()) << endl;
		getline(ppmImage, line);
	}

	cout << endl;
}

void getNextLine() {

	getline(ppmImage, line);

	if (lineIsComment())
	{
		printComments();
	}

}

int main(int argc, char const *argv[])
{

	/* PPM image example
	P3
	3 2
	255
	# The part above is the header
	# "P3" means this is a RGB color image in ASCII
	# "3 2" is the width and height of the image in pixels
	# "255" is the maximum value for each color(color depth)
	# The part below is image data: RGB triplets
	255   0   0     0 255   0     0   0 255
	255 255   0   255 255 255     0   0   0
	*/

	getNextLine();

	if (line == "P3")
	{
		cout << "Imagen en formato de 24 bits" << endl;
	}
	
	getNextLine();

	// To parse height and width we need the position of the space character
	string delim = " ";
	int delimIdx = line.find(delim);

	// Stoi parses str interpreting its content as an integral number of the specified base, which is returned as an int value.
	//  width = the first number from index 0 to space character index
	int w = stoi(line.substr(0, delimIdx), nullptr, 0);
	// height = the second number from space character index to line end
	int h = stoi(line.substr(delimIdx, line.length()), nullptr, 0);
	cout << "La imagen mide: " << h << " altura x " << w  << " anchura" << endl;
 
	getNextLine();
	// Maximum value for each color
	int colorDepth = stoi(line, nullptr, 0);
	cout << "Profundidad de bits: " << colorDepth << endl;
	
	getNextLine();

	return 0;
}