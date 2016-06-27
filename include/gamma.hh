#include <random>
#include <iostream>
#include <iomanip>
#include <queue>
#include <string>
#include <fstream>
#include <cstring>
#include <ctime>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <functional>

//using namespace std;

class gammaGenerator {
public:
	double k = 2.0;
	double theta = 3.0;
	gammaGenerator();
	~gammaGenerator();
	int events;
	void run(gammaGenerator g);
private:
};

gammaGenerator::gammaGenerator() {
	std::default_random_engine generator;
	std::gamma_distribution<double> distribution();
}

gammaGenerator::~gammaGenerator() {}

void gammaGenerator::run(gammaGenerator g) {
	FILE* test = fopen("test.txt","w");
	for (int i = 0; i < events; i++) {
		double number = distribution(g);
		number = number*(156/14);
		if (number < 157) sprintf(number,"\n");
	}
}