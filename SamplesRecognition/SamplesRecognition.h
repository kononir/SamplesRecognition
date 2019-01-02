#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#define ETA 0.8
#define PERIOD 2

typedef struct NeuralNetwork {
	double** trainingSamples;
	double** currWeightMatrix;

	int neuronsNumber;
	int samplesNumber;
	int reachedNumberOfTrainingSteps;
} NeuralNetwork;

double** readSampleFromFile(std::string filePath, int samplesNumber, int neuronsNumber);
void printSample(double* sample, int width, int height);

void trainNeuralNetwork(NeuralNetwork &neuralNetwork);
double* recognizeSample(NeuralNetwork neuralNetwork, double* noisySample);

double calculateS(double* X, double* Wi, int n);
double calculateY(double S, double prevY);