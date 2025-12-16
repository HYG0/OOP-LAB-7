#include "../include/factory.h"
#include "../include/npc.h"
#include "../include/observer.h"
#include <gtest/gtest.h>
#include <memory>

// Тестируем создание NPC
TEST(NPCTest, Creation) {
  auto squirrel = NPCFactory::createNPC("Squirrel", "TestSquirrel", 10, 20);
  ASSERT_NE(squirrel, nullptr);
  EXPECT_EQ(squirrel->getName(), "TestSquirrel");
  EXPECT_EQ(squirrel->getX(), 10);
  EXPECT_EQ(squirrel->getY(), 20);
  EXPECT_EQ(squirrel->getType(), "Squirrel");
  EXPECT_TRUE(squirrel->isAlive());
}

// Тестируем параметры перемещния NPC
TEST(NPCTest, MovementParameters) {
  auto squirrel = NPCFactory::createNPC("Squirrel", "S1", 0, 0);
  auto werewolf = NPCFactory::createNPC("Werewolf", "W1", 0, 0);
  auto druid = NPCFactory::createNPC("Druid", "D1", 0, 0);

  EXPECT_EQ(squirrel->getMoveDistance(), 20);
  EXPECT_EQ(squirrel->getKillDistance(), 10);

  EXPECT_EQ(werewolf->getMoveDistance(), 40);
  EXPECT_EQ(werewolf->getKillDistance(), 5);

  EXPECT_EQ(druid->getMoveDistance(), 10);
  EXPECT_EQ(druid->getKillDistance(), 10);
}

// Проверяем правила боя
TEST(NPCTest, BattleRules) {
  auto squirrel = NPCFactory::createNPC("Squirrel", "S1", 0, 0);
  auto werewolf = NPCFactory::createNPC("Werewolf", "W1", 0, 0);
  auto druid = NPCFactory::createNPC("Druid", "D1", 0, 0);

  // Белка убивает оборотня и друида
  EXPECT_TRUE(squirrel->canKill(*werewolf));
  EXPECT_TRUE(squirrel->canKill(*druid));

  // Оборотень убивает друида и не убивает белку
  EXPECT_TRUE(werewolf->canKill(*druid));
  EXPECT_FALSE(werewolf->canKill(*squirrel));

  // Друид никого не убвает
  EXPECT_FALSE(druid->canKill(*squirrel));
  EXPECT_FALSE(druid->canKill(*werewolf));
}

// Проверяем расчёт дистанции
TEST(NPCTest, DistanceCalculation) {
  auto npc1 = NPCFactory::createNPC("Squirrel", "S1", 0, 0);
  auto npc2 = NPCFactory::createNPC("Werewolf", "W1", 3, 4);

  EXPECT_DOUBLE_EQ(npc1->distanceTo(*npc2), 5.0);
  EXPECT_DOUBLE_EQ(npc2->distanceTo(*npc1), 5.0);
}

// Проверяем смерть NPC
TEST(NPCTest, Death) {
  auto npc = NPCFactory::createNPC("Squirrel", "S1", 0, 0);
  EXPECT_TRUE(npc->isAlive());

  npc->kill();
  EXPECT_FALSE(npc->isAlive());
}

// Тестируем бросание кубика
TEST(NPCTest, DiceRoll) {
  auto npc = NPCFactory::createNPC("Squirrel", "S1", 0, 0);

  for (int i = 0; i < 100; ++i) {
    int roll = npc->rollDice();
    EXPECT_GE(roll, 1);
    EXPECT_LE(roll, 6);
  }
}

// Проверяем невалидный тип
TEST(FactoryTest, InvalidType) {
  auto npc = NPCFactory::createNPC("InvalidType", "Test", 0, 0);
  EXPECT_EQ(npc, nullptr);
}

// Проверяем паттерн Observer
TEST(ObserverTest, SubjectNotification) {
  class TestSubject : public Subject {
  public:
    void triggerNotification(const std::string &msg) { notifyObservers(msg); }
  };

  class TestObserver : public Observer {
  public:
    std::string lastMessage;
    void notify(const std::string &message) override { lastMessage = message; }
  };

  TestSubject subject;
  TestObserver observer;
  subject.addObserver(&observer);

  subject.triggerNotification("Test notification");
  EXPECT_EQ(observer.lastMessage, "Test notification");
}
