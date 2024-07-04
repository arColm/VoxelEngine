
#ifndef EVENT_H
#define EVENT_H
#include <vector>
#include <functional>

template <class... T>
class Event {
	

private:
	std::vector<std::function<void(T...)>> ls;

public:
	Event<T...>& operator +=(std::function<void(T...)> fn) {
		ls.push_back(fn);
		return *this;
	}
	void addObserver(std::function<void(T...)>* fn) {
		ls.push_back(*fn);
	}

	Event<T...>& operator -=(std::function<void(T...)>* fn) {
		auto location = std::find(ls.begin(), ls.end(), fn);
		if (location != ls.end()) {
			ls.erase(location);
		}
		return *this;
	}
	

	void invoke(T... x) {
		for (auto fn : ls) {
			fn(x);
		}
	}

}; 
#endif