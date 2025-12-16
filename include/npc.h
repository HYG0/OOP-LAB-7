#pragma once
#include <string>
#include <memory>
#include <cmath>
#include <mutex>
#include <shared_mutex>
#include <random>
#include <mutex>
#include <shared_mutex>
#include <random>

class Visitor;

class NPC {
public:
    NPC(const std::string& name, int x, int y) : name_(name), x_(x), y_(y), alive_(true) {}
    virtual ~NPC() = default;
    
    virtual void accept(Visitor& visitor) = 0;
    virtual std::string getType() const = 0;
    virtual bool canKill(const NPC& other) const = 0;
    virtual int getMoveDistance() const = 0;
    virtual int getKillDistance() const = 0;
    
    const std::string& getName() const { return name_; }
    
    int getX() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return x_;
    }
    
    int getY() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return y_;
    }
    
    bool isAlive() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return alive_;
    }
    
    void kill() {
        std::lock_guard<std::shared_mutex> lock(mutex_);
        alive_ = false;
    }
    
    void move(int mapSize) {
        std::lock_guard<std::shared_mutex> lock(mutex_);
        if (!alive_) return;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(-getMoveDistance(), getMoveDistance());
        
        int newX = x_ + dis(gen);
        int newY = y_ + dis(gen);
        
        x_ = std::max(0, std::min(mapSize - 1, newX));
        y_ = std::max(0, std::min(mapSize - 1, newY));
    }
    
    double distanceTo(const NPC& other) const {
        std::shared_lock<std::shared_mutex> lock1(mutex_);
        std::shared_lock<std::shared_mutex> lock2(other.mutex_);
        int dx = x_ - other.x_;
        int dy = y_ - other.y_;
        return sqrt(dx * dx + dy * dy);
    }
    
    int rollDice() const {
        static thread_local std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> dis(1, 6);
        return dis(gen);
    }

protected:
    std::string name_;
    int x_, y_;
    bool alive_;
    mutable std::shared_mutex mutex_;
};

class Squirrel : public NPC {
public:
    Squirrel(const std::string& name, int x, int y) : NPC(name, x, y) {}
    void accept(Visitor& visitor) override;
    std::string getType() const override { return "Squirrel"; }
    bool canKill(const NPC& other) const override {
        return other.getType() == "Werewolf" || other.getType() == "Druid";
    }
    int getMoveDistance() const override { return 20; }
    int getKillDistance() const override { return 10; }
};

class Werewolf : public NPC {
public:
    Werewolf(const std::string& name, int x, int y) : NPC(name, x, y) {}
    void accept(Visitor& visitor) override;
    std::string getType() const override { return "Werewolf"; }
    bool canKill(const NPC& other) const override {
        return other.getType() == "Druid";
    }
    int getMoveDistance() const override { return 40; }
    int getKillDistance() const override { return 5; }
};

class Druid : public NPC {
public:
    Druid(const std::string& name, int x, int y) : NPC(name, x, y) {}
    void accept(Visitor& visitor) override;
    std::string getType() const override { return "Druid"; }
    bool canKill(const NPC& other) const override { return false; }
    int getMoveDistance() const override { return 10; }
    int getKillDistance() const override { return 10; }
};