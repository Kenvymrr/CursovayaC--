#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <string>
#include <iomanip>
#include <regex>

struct Message {
    std::string user;
    std::string time;
    std::string text;

    Message(const std::string& u, const std::string& t, const std::string& m)
        : user(u), time(t), text(m) {}

    bool operator<(const Message& other) const {
        if (time == other.time) {
            return user < other.user;
        }
        return time < other.time;
    }
};

class MessageStore {
public:
    void addMessage(const std::string& user, const std::string& time, const std::string& text) {
        messages.emplace(user, time, text);
    }

    void deleteMessage(const std::string& user, const std::string& time) {
        auto it = std::find_if(messages.begin(), messages.end(), [&](const Message& msg) {
            std::string cleanMsgTime = msg.time;
            cleanMsgTime.erase(std::remove_if(cleanMsgTime.begin(), cleanMsgTime.end(), ::isspace), cleanMsgTime.end());
            std::string cleanDelTime = time;
            cleanDelTime.erase(std::remove_if(cleanDelTime.begin(), cleanDelTime.end(), ::isspace), cleanDelTime.end());
            return msg.user == user && cleanMsgTime == cleanDelTime;
            });

        if (it != messages.end()) {
            messages.erase(it);
            std::cout << "Message deleted successfully." << std::endl;
        }
        else {
            std::cout << "Message not found for user: " << user << " at time: " << time << std::endl;
        }
    }

    void deleteMessagesByUser(const std::string& user) {
        auto it = messages.begin();
        while (it != messages.end()) {
            if (it->user == user) {
                it = messages.erase(it);
            }
            else {
                ++it;
            }
        }
        std::cout << "All messages from user: " << user << " have been deleted." << std::endl;
    }

    void printMessagesByUser(const std::string& user) const {
        for (const auto& msg : messages) {
            if (msg.user == user) {
                std::cout << msg.user << " " << msg.time << ": " << msg.text << std::endl;
            }
        }
    }

    void printMessagesByUserInRange(const std::string& user, const std::string& startTime, const std::string& endTime) const {
        for (const auto& msg : messages) {
            if (msg.user == user && msg.time >= startTime && msg.time <= endTime) {
                std::cout << msg.user << " " << msg.time << ": " << msg.text << std::endl;
            }
        }
    }

    void printMessagesInRange(const std::string& startTime, const std::string& endTime) const {
        for (const auto& msg : messages) {
            if (msg.time >= startTime && msg.time <= endTime) {
                std::cout << msg.user << " " << msg.time << ": " << msg.text << std::endl;
            }
        }
    }

    void printMessages() const {
        for (const auto& msg : messages) {
            std::cout << msg.user << " " << msg.time << ": " << msg.text << std::endl;
        }
    }

private:
    std::set<Message> messages;
};

void loadMessagesFromFile(const std::string& filename, MessageStore& store) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line;
    std::regex messageRegex(R"((\S+)\s+(\S+):\s+(.+))");
    std::smatch match;

    while (std::getline(file, line)) {
        if (std::regex_match(line, match, messageRegex)) {
            store.addMessage(match[1], match[2], match[3]);
        }
        else {
            std::cerr << "Invalid message format in file: " << filename << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file1> <file2> ..." << std::endl;
        return 1;
    }
    system("color F0");
    MessageStore store;

    for (int i = 1; i < argc; ++i) {
        loadMessagesFromFile(argv[i], store);
    }

    std::cout << "Loaded messages:" << std::endl;
    store.printMessages();

    std::string user = "user2";
    std::string time = "2024-06-01T10:16:31.456";
    std::string startTime = "2024-06-01T10:00:00.000";
    std::string endTime = "2024-06-01T12:00:00.000";

    std::cout << "\nMessages by user " << user << ":" << std::endl;
    store.printMessagesByUser(user);

    std::cout << "\nMessages by user " << user << " in range " << startTime << " to " << endTime << ":" << std::endl;
    store.printMessagesByUserInRange(user, startTime, endTime);

    std::cout << "\nMessages in range " << startTime << " to " << endTime << ":" << std::endl;
    store.printMessagesInRange(startTime, endTime);

    std::cout << std::endl;
    store.deleteMessage(user, time);
    std::cout << "Messages after deletion attempt:" << std::endl;
    store.printMessages();

    std::cout << std::endl;
    store.deleteMessagesByUser(user);
    std::cout << "Messages after deleting all messages by user " << user << ":" << std::endl;
    store.printMessages();

    return 0;
}
