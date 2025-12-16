#include "../include/game_engine.h"
#include "../include/observer.h"
#include <iostream>

int main() {
  std::cout << "=== Balagur Fate 3 Multithreaded Battle ===" << "\n";
  std::cout << "Starting game with 50 NPCs for 30 seconds..." << "\n";

  GameEngine engine;

  FileObserver fileObs;
  ConsoleObserver consoleObs(engine.getPrintMutex());
  engine.addObserver(&fileObs);
  engine.addObserver(&consoleObs);

  engine.start();

  return 0;
}
