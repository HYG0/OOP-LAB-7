#pragma once
#include "npc.h"
#include <memory>
#include <string>

class NPCFactory {
public:
    static std::unique_ptr<NPC> createNPC(const std::string& type, const std::string& name, int x, int y) {
        if (type == "Squirrel") return std::make_unique<Squirrel>(name, x, y);
        if (type == "Werewolf") return std::make_unique<Werewolf>(name, x, y);
        if (type == "Druid") return std::make_unique<Druid>(name, x, y);
        return nullptr;
    }
};