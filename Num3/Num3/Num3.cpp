#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <unordered_map>

enum CharacterClass { DEFENDER, HEALER, MELEE_FIGHTER, RANGED_FIGHTER };

enum SlotType { HELMET, SHOULDERS, GLOVES, CLOAK, WEAPON, NUM_SLOTS };

struct Stats {
    int health = 0;
    int armor = 0;
    int strength = 0;
    int intellect = 0;
    int agility = 0;
    int accuracy = 0;
    int luck = 0;
    int mastery = 0;
};

class Equipment {
public:
    Stats stats;
    SlotType slotType;
    std::string name;

    Equipment() : slotType(HELMET), name("Default") {}

    Equipment(SlotType slot, const std::string& name, int health, int armor, int main_stat, int secondary_stat1, int secondary_stat2, CharacterClass charClass)
        : slotType(slot), name(name) {
        stats.health = health;
        stats.armor = armor;

        if (charClass == DEFENDER) {
            stats.strength = main_stat;
        }
        else if (charClass == HEALER) {
            stats.intellect = main_stat;
        }
        else if (charClass == MELEE_FIGHTER) {
            stats.agility = main_stat;
        }
        else if (charClass == RANGED_FIGHTER) {
            stats.agility = main_stat;
        }

        stats.accuracy = secondary_stat1;
        stats.luck = secondary_stat1;
        stats.mastery = secondary_stat2;
    }
};

class Character {
public:
    std::string nickname;
    CharacterClass charClass;
    Stats baseStats;
    Stats totalStats;
    int abilityDamage;
    std::unordered_map<SlotType, Equipment> equipmentSlots;

    Character(const std::string& name, CharacterClass cls) : nickname(name), charClass(cls), abilityDamage(0) {
        baseStats = getDefaultStats(cls);
        totalStats = baseStats;
    }

    void equip(const Equipment& equipment) {
        equipmentSlots[equipment.slotType] = equipment;
        updateTotalStats();
    }

    void applyRandomAbility(std::vector<int>& abilityDamages) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, abilityDamages.size() - 1);
        int index = dis(gen);
        abilityDamage = abilityDamages[index];
    }

    void updateTotalStats() {
        totalStats = baseStats;
        for (const auto& eq : equipmentSlots) {
            totalStats.health += eq.second.stats.health;
            totalStats.armor += eq.second.stats.armor;
            totalStats.strength += eq.second.stats.strength;
            totalStats.intellect += eq.second.stats.intellect;
            totalStats.agility += eq.second.stats.agility;
            totalStats.accuracy += eq.second.stats.accuracy;
            totalStats.luck += eq.second.stats.luck;
            totalStats.mastery += eq.second.stats.mastery;
        }
    }

    int calculateDamage() const {
        int damage = 0;
        switch (charClass) {
        case DEFENDER:
            damage = totalStats.strength * 2;
            break;
        case HEALER:
            damage = totalStats.intellect * 2;
            break;
        case MELEE_FIGHTER:
            damage = totalStats.agility * 2 + totalStats.strength;
            break;
        case RANGED_FIGHTER:
            damage = totalStats.intellect * 2 + totalStats.agility;
            break;
        }
        damage += totalStats.mastery;
        damage += abilityDamage; // Применяем урон от способности
        return damage;
    }

private:
    Stats getDefaultStats(CharacterClass cls) {
        switch (cls) {
        case DEFENDER:
            return { 100, 50, 10, 5, 5, 5, 5, 5 };
        case HEALER:
            return { 80, 30, 5, 10, 5, 5, 5, 5 };
        case MELEE_FIGHTER:
            return { 90, 40, 10, 5, 10, 5, 5, 5 };
        case RANGED_FIGHTER:
            return { 70, 20, 5, 10, 10, 5, 5, 5 };
        default:
            return { 0, 0, 0, 0, 0, 0, 0, 0 };
            }
        }
};

class Game {
public:
    std::vector<Character> characters;
    std::default_random_engine generator;

    Game() : generator(std::random_device()()) {}

    Character createCharacter(const std::string& nickname, CharacterClass charClass) {
        return Character(nickname, charClass);
    }

    Equipment generateRandomEquipment(SlotType slot, CharacterClass charClass) {
        std::uniform_int_distribution<int> distribution(1, 10);
        int health = distribution(generator);
        int armor = distribution(generator);
        int main_stat = distribution(generator);
        int secondary_stat1 = distribution(generator);
        int secondary_stat2 = distribution(generator);

        std::string slotName;
        switch (slot) {
        case HELMET: slotName = "Helmet"; break;
        case SHOULDERS: slotName = "Shoulders"; break;
        case GLOVES: slotName = "Gloves"; break;
        case CLOAK: slotName = "Cloak"; break;
        case WEAPON: slotName = "Weapon"; break;
        default: slotName = "Unknown"; break;
        }

        return Equipment(slot, slotName, health, armor, main_stat, secondary_stat1, secondary_stat2, charClass);
    }

    void addCharacter(const Character& character) {
        characters.push_back(character);
    }

    void generateEquipmentsForCharacter(Character& character, int numEquipments) {
        std::uniform_int_distribution<int> slotDistribution(0, NUM_SLOTS - 1);
        for (int i = 0; i < numEquipments; ++i) {
            SlotType slot = static_cast<SlotType>(slotDistribution(generator));
            character.equip(generateRandomEquipment(slot, character.charClass));
        }
    }

    void addAbilities(std::vector<int>& abilityDamages) {
        abilityDamages.push_back(20);  
        abilityDamages.push_back(25);  
        abilityDamages.push_back(30);  
        abilityDamages.push_back(35);  
        abilityDamages.push_back(40);  
        abilityDamages.push_back(45);  
        abilityDamages.push_back(50);  
        abilityDamages.push_back(55);  
        abilityDamages.push_back(60);  
        abilityDamages.push_back(65);  
    }

    void showCharacterStats(const Character& character) const {
        std::cout << "Nickname: " << character.nickname << "\n";
        std::cout << "Class: ";
        switch (character.charClass) {
        case DEFENDER: std::cout << "Defender"; break;
        case HEALER: std::cout << "Healer"; break;
        case MELEE_FIGHTER: std::cout << "Melee Fighter"; break;
        case RANGED_FIGHTER: std::cout << "Ranged Fighter"; break;
        }
        std::cout << "\n";

        std::cout << "Character Stats:\n";
        std::cout << "Health: " << character.totalStats.health << "\n";
        std::cout << "Armor: " << character.totalStats.armor << "\n";
        std::cout << "Strength: " << character.totalStats.strength << "\n";
        std::cout << "Intellect: " << character.totalStats.intellect << "\n";
        std::cout << "Agility: " << character.totalStats.agility << "\n";
        std::cout << "Accuracy: " << character.totalStats.accuracy << "\n";
        std::cout << "Luck: " << character.totalStats.luck << "\n";
        std::cout << "Mastery: " << character.totalStats.mastery << "\n";
        std::cout << "Ability Damage: " << character.abilityDamage << "\n";
        std::cout << "Damage: " << character.calculateDamage() << "\n";

        std::cout << "Equipped Items:\n";
        for (const auto& eq : character.equipmentSlots) {
            std::cout << eq.second.name << " - Health: " << eq.second.stats.health << ", Armor: " << eq.second.stats.armor
                << ", Strength: " << eq.second.stats.strength << ", Intellect: " << eq.second.stats.intellect
                << ", Agility: " << eq.second.stats.agility << ", Accuracy: " << eq.second.stats.accuracy
                << ", Luck: " << eq.second.stats.luck << ", Mastery: " << eq.second.stats.mastery << "\n";
        }
    }

    void startGame() {
        std::vector<int> abilityDamages;
        addAbilities(abilityDamages);

        int numCharacters;
        std::cout << "Enter the number of characters you want to create: ";
        std::cin >> numCharacters;

        for (int i = 0; i < numCharacters; ++i) {
            std::string nickname;
            std::cout << "Enter the nickname for character " << i + 1 << ": ";
            std::cin >> nickname;

            std::cout << "Choose a character class for " << nickname << ":\n";
            std::cout << "1. Defender\n";
            std::cout << "2. Healer\n";
            std::cout << "3. Melee Fighter\n";
            std::cout << "4. Ranged Fighter\n";

            int choice;
            std::cin >> choice;

            CharacterClass charClass;
            switch (choice) {
            case 1:
                charClass = DEFENDER;
                break;
            case 2:
                charClass = HEALER;
                break;
            case 3:
                charClass = MELEE_FIGHTER;
                break;
            case 4:
                charClass = RANGED_FIGHTER;
                break;
            default:
                std::cout << "Incorrect choice. The default class is selected: Defender.\n";
                charClass = DEFENDER;
                break;
            }

            Character character = createCharacter(nickname, charClass);
            generateEquipmentsForCharacter(character, 3);
            character.applyRandomAbility(abilityDamages);
            addCharacter(character);
        }

        for (const auto& character : characters) {
            showCharacterStats(character);
            std::cout << "\n";
        }
    }
};

int main() {
    system("color F0");
    Game game;
    game.startGame();
    return 0;
}

