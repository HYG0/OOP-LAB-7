#pragma once
#include "npc.h"
#include "factory.h"
#include "observer.h"
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <iostream>

struct BattleTask {
    std::shared_ptr<NPC> attacker;
    std::shared_ptr<NPC> defender;
};

class GameEngine : public Subject {
public:
    GameEngine(int mapSize = 50);
    ~GameEngine();
    
    void start();
    void stop();
    
    std::mutex& getPrintMutex() { return printMutex_; }
    
private:
    void moveThread();
    void battleThread();
    void printThread();
    
    void generateNPCs();
    void printMap();
    
    static constexpr int MAP_SIZE = 30;
    static constexpr int NPC_COUNT = 50;
    static constexpr int GAME_DURATION = 30;
    
    std::vector<std::shared_ptr<NPC>> npcs_;
    std::queue<BattleTask> battleQueue_;
    
    mutable std::shared_mutex npcsMutex_;
    std::mutex battleQueueMutex_;
    std::mutex printMutex_;
    
    std::condition_variable battleCondition_;
    std::atomic<bool> running_;
    
    std::thread moveThread_;
    std::thread battleThread_;
    std::thread printThread_;
};