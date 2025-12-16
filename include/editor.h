#pragma once
#include "npc.h"
#include "factory.h"
#include "visitor.h"
#include "observer.h"
#include <vector>
#include <memory>
#include <fstream>
#include <iostream>
#include <cmath>

class DungeonEditor {
public:
    void addNPC(const std::string& type, const std::string& name, int x, int y) {
        if (x < 0 || x > 500 || y < 0 || y > 500) {
            std::cout << "Coordinates must be in range [0, 500]" << std::endl;
            return;
        }
        
        for (const auto& npc : npcs_) {
            if (npc->getX() == x && npc->getY() == y) {
                std::cout << "Position (" << x << ", " << y << ") is already occupied by " << npc->getName() << std::endl;
                return;
            }
        }
        
        auto npc = NPCFactory::createNPC(type, name, x, y);
        if (npc) {
            npcs_.push_back(std::move(npc));
            std::cout << "Added " << type << " " << name << " at (" << x << ", " << y << ")" << std::endl;
        }
    }
    
    void saveToFile(const std::string& filename) {
        std::ofstream file(filename);
        for (const auto& npc : npcs_) {
            file << npc->getType() << " " << npc->getName() << " " 
                 << npc->getX() << " " << npc->getY() << std::endl;
        }
        std::cout << "Saved " << npcs_.size() << " NPCs to " << filename << std::endl;
    }
    
    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        std::string type, name;
        int x, y;
        npcs_.clear();
        
        while (file >> type >> name >> x >> y) {
            auto npc = NPCFactory::createNPC(type, name, x, y);
            if (npc) npcs_.push_back(std::move(npc));
        }
        std::cout << "Loaded " << npcs_.size() << " NPCs from " << filename << std::endl;
    }
    
    void printNPCs() {
        std::cout << "\nNPCs in dungeon:" << std::endl;
        for (const auto& npc : npcs_) {
            std::cout << npc->getType() << " " << npc->getName() 
                     << " at (" << npc->getX() << ", " << npc->getY() << ")" << std::endl;
        }
    }
    
    void startBattle(double range) {
        BattleVisitor visitor(npcs_, range);
        FileObserver fileObs;
        ConsoleObserver consoleObs;
        visitor.addObserver(&fileObs);
        visitor.addObserver(&consoleObs);
        
        std::cout << "\nStarting battle with range " << range << std::endl;
        
        visitor.processBattle();
        
        std::cout << "Battle ended. " << npcs_.size() << " NPCs survived." << std::endl;
    }

private:
    std::vector<std::unique_ptr<NPC>> npcs_;
};