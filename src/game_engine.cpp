#include "../include/game_engine.h"
#include <random>
#include <algorithm>

GameEngine::GameEngine(int mapSize) : running_(false) {
    generateNPCs();
}

GameEngine::~GameEngine() {
    stop();
}

void GameEngine::start() {
    running_ = true;
    
    moveThread_ = std::thread([this]() { moveThread(); });
    battleThread_ = std::thread([this]() { battleThread(); });
    printThread_ = std::thread([this]() { printThread(); });
    
    std::this_thread::sleep_for(std::chrono::seconds(GAME_DURATION));
    stop();
}

void GameEngine::stop() {
    running_ = false;
    battleCondition_.notify_all();
    
    if (moveThread_.joinable()) moveThread_.join();
    if (battleThread_.joinable()) battleThread_.join();
    if (printThread_.joinable()) printThread_.join();
    
    {
        std::lock_guard<std::mutex> lock(printMutex_);
        std::cout << "\n=== SURVIVORS ===" << std::endl;
        std::shared_lock<std::shared_mutex> npcLock(npcsMutex_);
        for (const auto& npc : npcs_) {
            if (npc->isAlive()) {
                std::cout << npc->getType() << " " << npc->getName() 
                         << " at (" << npc->getX() << ", " << npc->getY() << ")" << std::endl;
            }
        }
    }
}

void GameEngine::moveThread() {
    while (running_) {
        {
            std::shared_lock<std::shared_mutex> lock(npcsMutex_);
            for (auto& npc : npcs_) {
                if (npc->isAlive()) {
                    npc->move(MAP_SIZE);
                }
            }
            
            for (size_t i = 0; i < npcs_.size(); ++i) {
                for (size_t j = i + 1; j < npcs_.size(); ++j) {
                    if (npcs_[i]->isAlive() && npcs_[j]->isAlive()) {
                        double dist = npcs_[i]->distanceTo(*npcs_[j]);
                        
                        if (npcs_[i]->canKill(*npcs_[j]) && dist <= npcs_[i]->getKillDistance()) {
                            std::lock_guard<std::mutex> battleLock(battleQueueMutex_);
                            battleQueue_.push({npcs_[i], npcs_[j]});
                            battleCondition_.notify_one();
                        }
                        
                        if (npcs_[j]->canKill(*npcs_[i]) && dist <= npcs_[j]->getKillDistance()) {
                            std::lock_guard<std::mutex> battleLock(battleQueueMutex_);
                            battleQueue_.push({npcs_[j], npcs_[i]});
                            battleCondition_.notify_one();
                        }
                    }
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void GameEngine::battleThread() {
    while (running_) {
        std::unique_lock<std::mutex> lock(battleQueueMutex_);
        battleCondition_.wait(lock, [this] { return !battleQueue_.empty() || !running_; });
        
        if (!running_) break;
        
        BattleTask task = battleQueue_.front();
        battleQueue_.pop();
        lock.unlock();
        
        if (task.attacker->isAlive() && task.defender->isAlive()) {
            int attackRoll = task.attacker->rollDice();
            std::this_thread::sleep_for(std::chrono::microseconds(1));
            int defenseRoll = task.defender->rollDice();
            
            if (attackRoll > defenseRoll) {
                task.defender->kill();
                std::string message = task.attacker->getName() + " (" + task.attacker->getType() + 
                                    ") killed " + task.defender->getName() + " (" + task.defender->getType() + 
                                    ") [" + std::to_string(attackRoll) + " vs " + std::to_string(defenseRoll) + "]";
                notifyObservers(message);
            }
        }
    }
}

void GameEngine::printThread() {
    while (running_) {
        printMap();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void GameEngine::generateNPCs() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> posDis(0, MAP_SIZE - 1);
    std::uniform_int_distribution<> typeDis(0, 2);
    
    std::vector<std::string> types = {"Squirrel", "Werewolf", "Druid"};
    
    for (int i = 0; i < NPC_COUNT; ++i) {
        std::string type = types[typeDis(gen)];
        std::string name = type + std::to_string(i);
        int x = posDis(gen);
        int y = posDis(gen);
        
        auto npc = NPCFactory::createNPC(type, name, x, y);
        if (npc) {
            npcs_.push_back(std::shared_ptr<NPC>(npc.release()));
        }
    }
}

void GameEngine::printMap() {
    std::lock_guard<std::mutex> lock(printMutex_);
    std::shared_lock<std::shared_mutex> npcLock(npcsMutex_);
    
    std::vector<std::vector<char>> map(MAP_SIZE, std::vector<char>(MAP_SIZE, '.'));
    
    int aliveCount = 0;
    for (const auto& npc : npcs_) {
        if (npc->isAlive()) {
            aliveCount++;
            int x = npc->getX();
            int y = npc->getY();
            char symbol = npc->getType()[0];
            map[y][x] = symbol;
        }
    }
    
    std::cout << "\n=== MAP (Alive: " << aliveCount << ") ===" << std::endl;
    for (int y = 0; y < MAP_SIZE; ++y) {
        for (int x = 0; x < MAP_SIZE; ++x) {
            std::cout << map[y][x];
        }
        std::cout << std::endl;
    }
}