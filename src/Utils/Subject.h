
#ifndef SUBJECT_H
#define SUBJECT_H
#include <vector>
#include "Observer.h"

class Subject {
	std::vector<Observer*> observers;


	template<typename... Args>
	void notify(Args... args) {
		for (Observer* o : observers) {
		}
	}
};

#endif