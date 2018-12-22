// SamplesRecognition.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main()
{
	string filePath;
	ifstream samplesFile;

	int sampleWidth, sampleHeight;

	try {
		cout << "Input sample width: ";
		cin >> sampleWidth;

		if (sampleWidth <= 0) {
			throw "Error - Invalid parameter: sample width";
		}

		cout << "Input sample height: ";
		cin >> sampleHeight;

		if (sampleHeight <= 0) {
			throw "Error - Invalid parameter: sample height";
		}

		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());

		cout << "Input file path: " << endl;
		getline(cin, filePath, '\n');

		samplesFile.open(filePath);

		if (!samplesFile.is_open()) {
			throw "Error - Failed to open: " + filePath;
		}



		samplesFile.close();
	}
	catch (const char* mesage) {
		cerr << mesage << endl;

		system("pause");
		return 1;
	}

	system("pause");
	return 0;
}
