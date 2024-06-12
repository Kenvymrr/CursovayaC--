#include <iostream>
#include <vector>
#include <forward_list>
#include <algorithm>
#include <random>
#include <ctime>
#include <unordered_map>

// Класс для представления билета
class Ticket
{
private:
    int number;
    double prize;

public:
    Ticket(int num) : number(num), prize(0) {}

    int getNumber() const { return number; }
    double getPrize() const { return prize; }

    void setPrize(double amount) { prize = amount; }
};

// Реализация односвязного списка
template<typename T>
class Node
{
public:
    T data;
    Node* next;

    Node(T value) : data(value), next(nullptr) {}
};

// Класс для реализации стека
template<typename T>
class Stack
{
private:
    Node<T>* top;

public:
    Stack() : top(nullptr) {}

    void push(T value)
    {
        Node<T>* newNode = new Node<T>(value);
        newNode->next = top;
        top = newNode;
    }

    T pop()
    {
        if (!top)
        {
            throw std::out_of_range("Stack is empty");
        }
        Node<T>* temp = top;
        top = top->next;
        T value = temp->data;
        delete temp;
        return value;
    }

    bool isEmpty() const
    {
        return top == nullptr;
    }
};

// Класс для реализации очереди
template<typename T>
class Queue
{
private:
    Node<T>* front;
    Node<T>* back;

public:
    Queue() : front(nullptr), back(nullptr) {}

    void enqueue(T value)
    {
        Node<T>* newNode = new Node<T>(value);
        if (back)
        {
            back->next = newNode;
        }
        back = newNode;
        if (!front)
        {
            front = back;
        }
    }

    T dequeue()
    {
        if (!front)
        {
            throw std::out_of_range("Queue is empty");
        }
        Node<T>* temp = front;
        front = front->next;
        T value = temp->data;
        delete temp;
        if (!front)
        {
            back = nullptr;
        }
        return value;
    }

    bool isEmpty() const
    {
        return front == nullptr;
    }
};

// Класс для представления лотереи с использованием std::vector
class LotteryVector
{
private:
    std::vector<Ticket> tickets;
    std::unordered_map<int, double> prizes;

public:
    // Генерация билетов для лотереи
    void generateTickets(int numTickets)
    {
        tickets.clear();
        for (int i = 1; i <= numTickets; ++i)
        {
            tickets.emplace_back(i); // Нумерация билетов с 1
        }
    }

    // Перемешивание билетов
    void shuffleTickets()
    {
        static std::mt19937 mt(std::time(nullptr)); // Генератор случайных чисел
        std::shuffle(tickets.begin(), tickets.end(), mt);
    }

    // Получение случайного билета
    Ticket getRandomTicket()
    {
        Ticket ticket = tickets.back();
        tickets.pop_back();
        return ticket;
    }

    // Поиск билета по номеру
    Ticket* findTicket(int number)
    {
        auto it = std::find_if(tickets.begin(), tickets.end(), [number](const Ticket& ticket) {
            return ticket.getNumber() == number;
            });

        if (it != tickets.end())
            return &(*it); // Возвращаем указатель на найденный билет
        else
            return nullptr; // Билет не найден
    }

    // Проверка участия билета в розыгрыше и получение суммы выигрыша
    double checkAndGetValue(const Ticket& ticket)
    {
        auto it = prizes.find(ticket.getNumber());
        if (it != prizes.end())
        {
            return it->second; // Возвращаем выигрыш билета
        }
        else
        {
            return 0; // Билет не участвовал в розыгрыше
        }
    }

    void setPrize(const Ticket& ticket, double prize)
    {
        prizes[ticket.getNumber()] = prize;
    }

    bool hasTickets() const
    {
        return !tickets.empty();
    }
};

// Класс для представления лотереи с использованием std::forward_list
class LotteryForwardList
{
private:
    std::forward_list<Ticket> tickets;
    std::unordered_map<int, double> prizes;

public:
    // Генерация билетов для лотереи
    void generateTickets(int numTickets)
    {
        tickets.clear();
        for (int i = numTickets; i >= 1; --i)
        {
            tickets.emplace_front(i); // Нумерация билетов с 1
        }
    }

    // Перемешивание билетов
    void shuffleTickets()
    {
        std::vector<Ticket> tempTickets{ std::begin(tickets), std::end(tickets) };
        static std::mt19937 mt(std::time(nullptr)); // Генератор случайных чисел
        std::shuffle(tempTickets.begin(), tempTickets.end(), mt);
        tickets.assign(tempTickets.begin(), tempTickets.end());
    }

    // Получение случайного билета
    Ticket getRandomTicket()
    {
        Ticket ticket = tickets.front();
        tickets.pop_front();
        return ticket;
    }

    // Поиск билета по номеру
    Ticket* findTicket(int number)
    {
        auto it = std::find_if(tickets.begin(), tickets.end(), [number](const Ticket& ticket) {
            return ticket.getNumber() == number;
            });

        if (it != tickets.end())
            return &(*it); // Возвращаем указатель на найденный билет
        else
            return nullptr; // Билет не найден
    }

    // Проверка участия билета в розыгрыше и получение суммы выигрыша
    double checkAndGetValue(const Ticket& ticket)
    {
        auto it = prizes.find(ticket.getNumber());
        if (it != prizes.end())
        {
            return it->second; // Возвращаем выигрыш билета
        }
        else
        {
            return 0; // Билет не участвовал в розыгрыше
        }
    }

    void setPrize(const Ticket& ticket, double prize)
    {
        prizes[ticket.getNumber()] = prize;
    }

    bool hasTickets() const
    {
        return !tickets.empty();
    }
};

// Класс для представления тиража с использованием стека
class LotteryDrawStack
{
private:
    Stack<Ticket> winningTickets;
    std::unordered_map<int, double> prizes;

public:
    // Проведение розыгрыша
    void conductDraw(LotteryVector& lottery, int numWinners)
    {
        lottery.shuffleTickets(); // Перемешивание билетов перед розыгрышем
        static std::mt19937 mt(std::time(nullptr)); // Генератор случайных чисел
        std::uniform_int_distribution<int> dist(0, 10000);

        for (int i = 0; i < numWinners && lottery.hasTickets(); ++i)
        {
            Ticket ticket = lottery.getRandomTicket();
            double prize = dist(mt);
            ticket.setPrize(prize); // Пример выигрыша
            winningTickets.push(ticket);
            prizes[ticket.getNumber()] = prize; // Сохранение выигрыша
        }
    }

    // Вывод всех номеров билетов победителей и их выигрышей
    void printWinningTickets()
    {
        Stack<Ticket> tempStack = winningTickets;
        std::vector<Ticket> sortedTickets;

        while (!tempStack.isEmpty())
        {
            sortedTickets.push_back(tempStack.pop());
        }

        // Сортировка билетов по величине выигрыша в порядке убывания
        std::sort(sortedTickets.begin(), sortedTickets.end(), [](const Ticket& a, const Ticket& b) {
            return a.getPrize() > b.getPrize();
            });

        std::cout << "Winning tickets and their prizes (Stack):\n";
        for (const auto& ticket : sortedTickets)
        {
            std::cout << "Ticket " << ticket.getNumber() << " won $" << ticket.getPrize() << std::endl;
        }
    }

    // Поиск билета по номеру и получение суммы выигрыша
    double checkAndGetValue(int ticketNumber)
    {
        auto it = prizes.find(ticketNumber);
        if (it != prizes.end())
        {
            return it->second; // Возвращаем выигрыш билета
        }
        else
        {
            return 0; // Билет не участвовал в розыгрыше
        }
    }
};

// Класс для представления тиража с использованием очереди
class LotteryDrawQueue
{
private:
    Queue<Ticket> winningTickets;
    std::unordered_map<int, double> prizes;

public:
    // Проведение розыгрыша
    void conductDraw(LotteryForwardList& lottery, int numWinners)
    {
        lottery.shuffleTickets(); // Перемешивание билетов перед розыгрышем
        static std::mt19937 mt(std::time(nullptr)); // Генератор случайных чисел
        std::uniform_int_distribution<int> dist(0, 10000);

        for (int i = 0; i < numWinners && lottery.hasTickets(); ++i)
        {
            Ticket ticket = lottery.getRandomTicket();
            double prize = dist(mt);
            ticket.setPrize(prize); // Пример выигрыша
            winningTickets.enqueue(ticket);
            prizes[ticket.getNumber()] = prize; // Сохранение выигрыша
        }
    }

    // Вывод всех номеров билетов победителей и их выигрышей
    void printWinningTickets()
    {
        Queue<Ticket> tempQueue = winningTickets;
        std::vector<Ticket> sortedTickets;

        while (!tempQueue.isEmpty())
        {
            sortedTickets.push_back(tempQueue.dequeue());
        }

        // Сортировка билетов по величине выигрыша в порядке убывания
        std::sort(sortedTickets.begin(), sortedTickets.end(), [](const Ticket& a, const Ticket& b) {
            return a.getPrize() > b.getPrize();
            });

        std::cout << "Winning tickets and their prizes (Queue):\n";
        for (const auto& ticket : sortedTickets)
        {
            std::cout << "Ticket " << ticket.getNumber() << " won $" << ticket.getPrize() << std::endl;
        }
    }

    // Поиск билета по номеру и получение суммы выигрыша
    double checkAndGetValue(int ticketNumber)
    {
        auto it = prizes.find(ticketNumber);
        if (it != prizes.end())
        {
            return it->second; // Возвращаем выигрыш билета
        }
        else
        {
            return 0; // Билет не участвовал в розыгрыше
        }
    }
};

int main()
{
    system("color F0");

    unsigned long count_ticket;
    int choice;

    std::cout << "Enter count tickets: ";
    std::cin >> count_ticket;

    std::cout << "Choose mode of operation:\n"
        << "1. Stack\n"
        << "2. Forward List\n"
        << "3. Queue\n"
        << "4. Vector\n";
    std::cin >> choice;

    LotteryVector lotteryVector;
    LotteryForwardList lotteryForwardList;
    LotteryDrawStack drawStack;
    LotteryDrawQueue drawQueue;

    switch (choice)
    {
    case 1:
        lotteryVector.generateTickets(count_ticket);
        drawStack.conductDraw(lotteryVector, 5);
        drawStack.printWinningTickets();
        break;
    case 2:
        lotteryForwardList.generateTickets(count_ticket);
        drawQueue.conductDraw(lotteryForwardList, 5);
        drawQueue.printWinningTickets();
        break;
    case 3:
        lotteryForwardList.generateTickets(count_ticket);
        drawQueue.conductDraw(lotteryForwardList, 5);
        drawQueue.printWinningTickets();
        break;
    case 4:
        lotteryVector.generateTickets(count_ticket);
        drawStack.conductDraw(lotteryVector, 5);
        drawStack.printWinningTickets();
        break;
    default:
        std::cout << "Invalid choice!" << std::endl;
        return 1;
    }

    // Поиск и проверка билетов
    int ticketNumber;
    std::cout << "Enter ticket number to search: ";
    std::cin >> ticketNumber;

    double prize = 0;
    switch (choice)
    {
    case 1:
        prize = drawStack.checkAndGetValue(ticketNumber);
        break;
    case 2:
        prize = drawQueue.checkAndGetValue(ticketNumber);
        break;
    case 3:
        prize = drawQueue.checkAndGetValue(ticketNumber);
        break;
    case 4:
        prize = drawStack.checkAndGetValue(ticketNumber);
        break;
    default:
        std::cout << "Invalid choice!" << std::endl;
        return 1;
    }

    if (prize > 0)
    {
        std::cout << "Ticket " << ticketNumber << " won $" << prize << "." << std::endl;
    }
    else
    {
        std::cout << "Ticket " << ticketNumber << " did not win any prize or not found." << std::endl;
    }

    return 0;
}
