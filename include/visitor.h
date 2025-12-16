#pragma once
#include "observer.h"
#include <vector>
#include <memory>
#include <algorithm>

class NPC;
class Squirrel;
class Werewolf;
class Druid;

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(Squirrel& squirrel) = 0;
    virtual void visit(Werewolf& werewolf) = 0;
    virtual void visit(Druid& druid) = 0;
};

class BattleVisitor : public Visitor, public Subject {
public:
    BattleVisitor(std::vector<std::unique_ptr<NPC>>& npcs, double range) 
        : npcs_(npcs), range_(range) {}
    
    void visit(Squirrel& squirrel) override { battle(squirrel); }
    void visit(Werewolf& werewolf) override { battle(werewolf); }
    void visit(Druid& druid) override { battle(druid); }
    
    void processBattle() {
        std::vector<size_t> toRemove;
        
        for (size_t i = 0; i < npcs_.size(); ++i) {
            for (size_t j = 0; j < npcs_.size(); ++j) {
                if (i != j && npcs_[i]->distanceTo(*npcs_[j]) <= range_) {
                    if (npcs_[i]->canKill(*npcs_[j])) {
                        notifyObservers(npcs_[i]->getName() + " (" + npcs_[i]->getType() + 
                                      ") killed " + npcs_[j]->getName() + " (" + npcs_[j]->getType() + ")");
                        toRemove.push_back(j);
                    }
                }
            }
        }
        
        std::sort(toRemove.begin(), toRemove.end());
        toRemove.erase(std::unique(toRemove.begin(), toRemove.end()), toRemove.end());
        std::sort(toRemove.rbegin(), toRemove.rend());
        
        for (size_t idx : toRemove) {
            npcs_.erase(npcs_.begin() + idx);
        }
    }
    
private:
    void battle(NPC& attacker) {}
    
    std::vector<std::unique_ptr<NPC>>& npcs_;
    double range_;
};