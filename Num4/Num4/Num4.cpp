#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <sstream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <stack>
#include <functional>
#include <limits>

class SynonymDictionary {
private:
    std::unordered_map<std::string, std::string> synonym_map;
    std::unordered_map<std::string, std::vector<std::string>> canonical_map;

public:
    SynonymDictionary() = default;
    ~SynonymDictionary() = default;

    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        }

        std::string line;
        while (std::getline(file, line)) {
            parseLine(line);
        }
        file.close();
    }

    void saveToFile(const std::string& filename) const {
        std::ofstream file(filename);

        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        }

        for (const auto& entry : canonical_map) {
            file << entry.first << "{";
            for (size_t i = 0; i < entry.second.size(); ++i) {
                file << entry.second[i];
                if (i < entry.second.size() - 1) {
                    file << ", ";
                }
            }
            file << "}\n";
        }
        file.close();
    }

    std::string getCanonicalWord(const std::string& word) const {
        auto it = synonym_map.find(word);
        return it != synonym_map.end() ? it->second : word;
    }

    void addEntry(const std::string& canonical_word, const std::vector<std::string>& synonyms) {
        for (const auto& synonym : synonyms) {
            synonym_map[synonym] = canonical_word;
        }
        canonical_map[canonical_word] = synonyms;
    }

    void addSynonym(const std::string& canonical_word, const std::string& synonym) {
        synonym_map[synonym] = canonical_word;
        canonical_map[canonical_word].push_back(synonym);
    }

    void removeSynonym(const std::string& canonical_word, const std::string& synonym) {
        auto it = synonym_map.find(synonym);
        if (it != synonym_map.end() && it->second == canonical_word) {
            synonym_map.erase(it);
            auto& synonyms = canonical_map[canonical_word];
            synonyms.erase(std::remove(synonyms.begin(), synonyms.end(), synonym), synonyms.end());
        }
    }

    void parseLine(const std::string& line) {
        std::istringstream stream(line);
        std::string canonical_word;

        if (std::getline(stream, canonical_word, '{')) {
            std::string synonyms_list;

            if (std::getline(stream, synonyms_list, '}')) {
                std::istringstream synonyms_stream(synonyms_list);
                std::string synonym;
                std::vector<std::string> synonyms;

                while (std::getline(synonyms_stream, synonym, ',')) {
                    synonym.erase(0, synonym.find_first_not_of(" \t\n\r"));
                    synonym.erase(synonym.find_last_not_of(" \t\n\r") + 1);
                    synonym_map[synonym] = canonical_word;
                    synonyms.push_back(synonym);
                }

                canonical_map[canonical_word] = synonyms;
            }
        }
    }
};

class TextProcessor {
private:
    SynonymDictionary& dictionary;

public:
    TextProcessor(SynonymDictionary& dict) : dictionary(dict) {}

    void processFile(const std::string& input_filename, const std::string& output_filename, bool automatic_mode) {
        std::ifstream input_file(input_filename);
        if (!input_file.is_open()) {
            throw std::runtime_error("Could not open input file: " + input_filename);
        }

        std::ofstream output_file(output_filename);

        if (!output_file.is_open()) {
            input_file.close();
            throw std::runtime_error("Could not open output file: " + output_filename);
        }

        std::string line;
        while (std::getline(input_file, line)) {
            std::istringstream line_stream(line);
            std::string word;
            while (line_stream >> word) {
                std::string canonical_word = dictionary.getCanonicalWord(word);
                if (canonical_word == word && dictionary.getCanonicalWord(word) == word) {
                    if (!automatic_mode) {
                        std::cout << "Word not in dictionary: " << word << std::endl;
                        char option;
                        std::cout << "Add to dictionary? (y/n): ";
                        std::cin >> option;
                        if (option == 'y' || option == 'Y') {
                            std::cout << "Enter canonical word for " << word << ": ";
                            std::string canon;
                            std::cin >> canon;
                            dictionary.addSynonym(canon, word);
                        }
                    }
                }
                output_file << dictionary.getCanonicalWord(word) << " ";
            }
            output_file << "\n";
        }
        input_file.close();
        output_file.close();
    }
};

class UndoManager {
private:
    std::stack<std::function<void()>> undo_stack;

public:
    void addUndoAction(const std::function<void()>& action) {
        undo_stack.push(action);
    }

    void undoLastActions(int N) {
        for (int i = 0; i < N && !undo_stack.empty(); ++i) {
            undo_stack.top()();
            undo_stack.pop();
        }
    }

    bool isEmpty() const {
        return undo_stack.empty();
    }
};

void inputSynonym(SynonymDictionary& dict, UndoManager& undoManager) {
    std::string canon_word;
    std::string synonym;

    std::cout << "Enter canonical word: ";
    std::cin >> canon_word;
    std::cout << "Enter synonym: ";
    std::cin >> synonym;

    // Сохраняем текущее состояние словаря
    auto currentState = dict;

    // Добавляем действие отмены
    undoManager.addUndoAction([currentState, &dict]() {
        dict = currentState;
        });

    // Добавляем синоним в словарь
    dict.addSynonym(canon_word, synonym);
    dict.saveToFile("synonyms.txt");
}

void addNewWord(SynonymDictionary& dict, UndoManager& undoManager) {
    std::string canon_word;
    std::vector<std::string> synonyms;
    std::string synonym;

    std::cout << "Enter canonical word: ";
    std::cin >> canon_word;

    std::cout << "Enter synonyms (type 'end' to finish): ";
    while (std::cin >> synonym && synonym != "end") {
        synonyms.push_back(synonym);
    }

    undoManager.addUndoAction([canon_word, synonyms, &dict]() {
        for (const auto& s : synonyms) {
            dict.removeSynonym(canon_word, s);
        }
        // Сохранение словаря в файл после отмены действия
        dict.saveToFile("synonyms.txt");
        });

    dict.addEntry(canon_word, synonyms);
    // Сохранение словаря в файл после добавления нового слова
    dict.saveToFile("synonyms.txt");
}

void processText(bool learning_mode, SynonymDictionary& dict, UndoManager& undoManager) {
    std::string input_filename = "input.txt";
    std::string output_filename = "output.txt";

    TextProcessor processor(dict);
    std::unordered_set<std::string> unknown_words;

    // Обработка файла
    processor.processFile(input_filename, output_filename, learning_mode);

    // Если режим обучения и есть неизвестные слова
    if (learning_mode && !unknown_words.empty()) {
        std::cout << "Learning mode activated. Adding unknown words to dictionary..." << std::endl;
        for (const auto& word : unknown_words) {
            char option;
            std::cout << "Unknown word found: " << word << std::endl;
            std::cout << "Do you want to add it to the dictionary? (y/n): ";
            std::cin >> option;
            if (option == 'y' || option == 'Y') {
                std::string canonical_word;
                std::cout << "Enter canonical word for " << word << ": ";
                std::cin >> canonical_word;
                if (dict.getCanonicalWord(word) != word) {
                    std::cout << "This word already exists in the dictionary. Do you want to replace it? (y/n): ";
                    std::cin >> option;
                    if (option == 'y' || option == 'Y') {
                        dict.removeSynonym(dict.getCanonicalWord(word), word);
                        dict.addSynonym(canonical_word, word);
                    }
                }
                else {
                    dict.addSynonym(canonical_word, word);
                }
            }
        }
    }
}



int main() {
    system("color F0");
    const int MAX_UNDO = 10;

    SynonymDictionary dict;
    UndoManager undoManager;

    try {
        dict.loadFromFile("synonyms.txt");
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    while (true) {
        std::cout << "1. Process text (automatic mode)" << std::endl;
        std::cout << "2. Process text (learning mode)" << std::endl;
        std::cout << "3. Add a new synonym" << std::endl;
        std::cout << "4. Add a new canonical word with synonyms" << std::endl;
        std::cout << "5. Undo last action" << std::endl;
        std::cout << "6. Save and exit" << std::endl;

        int choice;
        std::cout << "Enter option: ";
        std::cin >> choice;

        if (std::cin.fail()) { // Проверка на ошибочный ввод
            std::cin.clear(); // Сброс флагов ошибок
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Очистка входного буфера
            std::cout << "Invalid input. Please enter a number." << std::endl;
            continue; // Переход к следующей итерации цикла
        }

        try {
            switch (choice) {
            case 1:
                processText(true, dict, undoManager);
                break;
            case 2:
                processText(false, dict, undoManager);
                break;
            case 3:
                inputSynonym(dict, undoManager);
                break;
            case 4:
                addNewWord(dict, undoManager);
                break;
            case 5: {
                int N;
                std::cout << "Enter number of actions to undo: ";
                std::cin >> N;
                if (N > MAX_UNDO) {
                    std::cout << "Cannot undo more than " << MAX_UNDO << " actions." << std::endl;
                    N = MAX_UNDO;
                }
                undoManager.undoLastActions(N);
                break;
            }
            case 6:
                dict.saveToFile("synonyms.txt");
                return 0;
            default:
                std::cout << "Unknown option! Please try again." << std::endl;
                break;
            }
        }
        catch (const std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    }
}
