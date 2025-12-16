#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <mutex>
#include <mutex>

class Observer {
public:
    virtual ~Observer() = default;
    virtual void notify(const std::string& message) = 0;
};

class FileObserver : public Observer {
public:
    void notify(const std::string& message) override {
        std::ofstream file("log.txt", std::ios::app);
        file << message << std::endl;
    }
};

class ConsoleObserver : public Observer {
public:
    ConsoleObserver(std::mutex& printMutex) : printMutex_(printMutex) {}
    
    void notify(const std::string& message) override {
        std::lock_guard<std::mutex> lock(printMutex_);
        std::cout << message << std::endl;
    }
    
private:
    std::mutex& printMutex_;
};

class Subject {
public:
    void addObserver(Observer* observer) {
        observers_.push_back(observer);
    }
    
    void notifyObservers(const std::string& message) {
        for (auto observer : observers_) {
            observer->notify(message);
        }
    }

private:
    std::vector<Observer*> observers_;
};