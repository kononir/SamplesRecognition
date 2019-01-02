// SamplesRecognition.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "SamplesRecognition.h"

using namespace std;

/*
* author: Novitskiy Vladislav
* group: 621701
* description: Главная функция; в ней происходит ввод входных параметров сети и вызываются остальные функции
*/
int main()
{
	string filePath;

	int sampleWidth, sampleHeight, noisySamplesNumber;

	double** noisySamples;

	NeuralNetwork neuralNetwork;

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

		neuralNetwork.neuronsNumber = sampleWidth * sampleHeight;

		cout << "Input training samples number (L <= " 
			<< (int)(neuralNetwork.neuronsNumber / 4 / log(neuralNetwork.neuronsNumber))
			<< "): ";
		cin >> neuralNetwork.samplesNumber;

		if (neuralNetwork.samplesNumber <= 0) {
			throw "Error - Invalid parameter: training samples number";
		}

		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());

		cout << "Input path of file with training samples: " << endl;
		getline(cin, filePath, '\n');

		neuralNetwork.trainingSamples 
			= readSampleFromFile(filePath, neuralNetwork.samplesNumber, neuralNetwork.neuronsNumber);

		cout << "Input noisy samples number: ";
		cin >> noisySamplesNumber;

		if (noisySamplesNumber <= 0) {
			throw "Error - Invalid parameter: noisy samples number";
		}

		cin.clear();
		cin.ignore(cin.rdbuf()->in_avail());

		cout << "Input path of file with noisy samples: " << endl;
		getline(cin, filePath, '\n');

		noisySamples 
			= readSampleFromFile(filePath, noisySamplesNumber, neuralNetwork.neuronsNumber);
	}
	catch (const char* mesage) {
		cerr << mesage << endl;

		system("pause");
		return 1;
	}

	cout << endl << "Recognizing samples: " << endl;

	for (int currSampleNumber = 0; currSampleNumber < neuralNetwork.samplesNumber; currSampleNumber++) {
		printSample(neuralNetwork.trainingSamples[currSampleNumber], sampleWidth, sampleHeight);
	}

	cout << "Noisy samples: " << endl;

	for (int currSampleNumber = 0; currSampleNumber < noisySamplesNumber; currSampleNumber++) {
		printSample(noisySamples[currSampleNumber], sampleWidth, sampleHeight);
	}

	trainNeuralNetwork(neuralNetwork);

	cout << "Recognized samples: " << endl;

	double* recognizedSample;
	for (int currSampleNumber = 0; currSampleNumber < noisySamplesNumber; currSampleNumber++) {
		recognizedSample = recognizeSample(neuralNetwork, noisySamples[currSampleNumber]);

		printSample(recognizedSample, sampleWidth, sampleHeight);
	}

	cout << "Reached number of training steps: " << neuralNetwork.reachedNumberOfTrainingSteps << endl << endl;

	system("pause");
	return 0;
}


/*
* author: Novitskiy Vladislav
* group: 621701
* description: Функция чтения образов из файла
*/
double** readSampleFromFile(string filePath, int samplesNumber, int neuronsNumber) {
	double** samples = new double*[samplesNumber];
	ifstream samplesFile;

	samplesFile.open(filePath);

	if (!samplesFile.is_open()) {
		throw "Error - Failed to open: " + filePath;
	}

	for (int currSampleNumber = 0; currSampleNumber < samplesNumber; currSampleNumber++) {
		samples[currSampleNumber] = new double[neuronsNumber];

		for (int currSampleComponent = 0; currSampleComponent < neuronsNumber; currSampleComponent++) {
			if (samplesFile.eof()) {
				throw "Error - Reached end of file: " + filePath;
			}

			samplesFile >> samples[currSampleNumber][currSampleComponent];
		}
	}

	samplesFile.close();

	return samples;
}


/*
* author: Novitskiy Vladislav
* group: 621701
* description: Функция печати на экран псевдографических образов
*/
void printSample(double* sample, int width, int height) {
	int currComponent = 0;

	printf("\n");

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			printf("%2d ", (int)sample[currComponent]);

			currComponent++;
		}

		printf("\n");
	}

	printf("\n");
}


/*
* author: Novitskiy Vladislav
* group: 621701
* description: Функция обучения релаксационной нейронной сети
*/
void trainNeuralNetwork(NeuralNetwork &neuralNetwork) {
	int &n = neuralNetwork.neuronsNumber;
	int &L = neuralNetwork.samplesNumber;
	int currCheckingSampleNumber;
	int currPer;
	int numOfSteps = 0;

	double** W;
	double** Q = neuralNetwork.trainingSamples;

	double* X;
	double* temp = new double[n];
	double* state;

	double etaN = ETA / n;

	bool networkTrained;
	bool currSampleRecognized;

	W = new double*[n];
	for (int currSampleNumber = 0; currSampleNumber < n; currSampleNumber++) {
		W[currSampleNumber] = new double[n];
		memset(W[currSampleNumber], 0, sizeof(double) * n);
	}

	do {
		for (int currSampleNumber = 0; currSampleNumber < L; currSampleNumber++) {
			X = Q[currSampleNumber];

			for (int i = 0; i < n; i++) {
				temp[i] = X[i];

				for (int j = 0; j < n; j++) {
					temp[i] -= W[i][j] * X[j];
				}

				temp[i] *= etaN;
			}

			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					if (i != j) {
						W[i][j] += temp[i] * X[j];
					}
					else {
						W[i][j] = 0;
					}
				}
			}
		}

		for (currCheckingSampleNumber = 0; currCheckingSampleNumber < L; currCheckingSampleNumber++) {
			X = Q[currCheckingSampleNumber];
			state = Q[currCheckingSampleNumber];

			currPer = 0;
			currSampleRecognized = true;

			do {
				for (int i = 0; i < n; i++) {
					state[i] = calculateY(calculateS(state, W[i], n), state[i]);

					currSampleRecognized = currSampleRecognized && (state[i] == X[i]);
				}

				currPer++;
			} while (currPer != PERIOD && !currSampleRecognized);

			if (!currSampleRecognized) {
				break;
			}
		}

		networkTrained = currCheckingSampleNumber == L;

		numOfSteps++;
	} while (!networkTrained);

	neuralNetwork.currWeightMatrix = W;
	neuralNetwork.reachedNumberOfTrainingSteps = numOfSteps;
}


/*
* author: Novitskiy Vladislav
* group: 621701
* description: Функция распознавания образов
*/
double* recognizeSample(NeuralNetwork neuralNetwork, double* noisySample) {
	int n = neuralNetwork.neuronsNumber;
	
	double** W = neuralNetwork.currWeightMatrix;

	double* recognizedSample = new double[n];

	double S;

	for (int i = 0; i < n; i++) {
		S = calculateS(noisySample, W[i], n);

		recognizedSample[i] = calculateY(S, noisySample[i]);
	}

	return recognizedSample;
}


/*
* author: Novitskiy Vladislav
* group: 621701
* description: Функция вычисления выхода функции синаптических преобразований нейронов сети 
*/
double calculateS(double* X, double* Wi, int n) {
	double S = 0;

	for (int j = 0; j < n; j++) {
		S += X[j] * Wi[j];
	}

	return S;
}


/*
* author: Novitskiy Vladislav
* group: 621701
* description: Функция вычисления выхода функции активации нейронов сети
*/
double calculateY(double S, double prevY) {
	double y;
	
	if (S > 0) {
		y = 1;
	}
	else if (S < 0) {
		y = -1;
	}
	else {
		y = prevY;
	}

	return y;
}
