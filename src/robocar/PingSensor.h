#ifndef PINGSENSOR_H
#define PINGSENSOR_H


#include "NewPing.h"

#define NUMOFPINGS 3

class PingSensor
{
public:
	NewPing sonar;
	unsigned int maxDistance;
	int pings[NUMOFPINGS];
	int sum;
	int index;
public:
	PingSensor(int trigPin, int echoPin, int MaxDistance):sonar(trigPin, echoPin, MaxDistance)
	{
		maxDistance = MaxDistance;
		sum = 0;
		index = 0;

		for(int i = 0; i < NUMOFPINGS; i++)
		{
			pings[i] = 0;
		}
	}
	int getPingDistance()
	{
		int pingDistance = sonar.ping_cm();
		if(pingDistance <= 0)
			return maxDistance;
		return pingDistance;
	};

	int averageDistances(int pingDistance)
	{
		sum = sum - pings[index] + pingDistance;
		pings[index++] = pingDistance;
		if(index >= NUMOFPINGS)
			index = 0;
		return sum / NUMOFPINGS;
	}

};

#endif
