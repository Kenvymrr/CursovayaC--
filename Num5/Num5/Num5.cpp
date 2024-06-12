#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

using namespace std;

struct Contract
{
    string number;
    string startDate;
    string endDate;
    string work;
    int cost;
};

struct Employee
{
    string lastName;
    string firstName;
    string middleName;
    vector<Contract> contracts;
};

map<string, Employee> employees; // Ассоциативный контейнер для хранения информации о сотрудниках 

void parseAndStoreData(const string& filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Failed to open the file: " << filename << endl;
        return;
    }

    string line;
    Employee currentEmployee;

    while (getline(file, line))
    {
        if (line.empty()) continue;

        // Считываем ФИО сотрудника
        stringstream ss(line);
        ss >> currentEmployee.lastName >> currentEmployee.firstName >> currentEmployee.middleName;

        // Считываем блок с информацией о договорах сотрудника
        getline(file, line); // Пропускаем открывающую фигурную скобку
        while (getline(file, line))
        {
            if (line == "}") break; // Конец блока

            // Считываем информацию о договоре
            Contract contract;
            stringstream ss_contract(line);
            string temp;
            ss_contract >> temp; // Пропускаем "Договор"
            ss_contract >> temp; // Считываем номер договора
            contract.number = temp;
            ss_contract >> temp; // Пропускаем "нач."
            ss_contract >> contract.startDate;
            ss_contract >> temp; // Пропускаем "кон."
            ss_contract >> contract.endDate;
            ss_contract >> temp; // Пропускаем "Работа"
            ss_contract >> contract.work;
            ss_contract >> temp; // Пропускаем "Стоимость"
            ss_contract >> contract.cost;

            currentEmployee.contracts.push_back(contract);
        }

        // Сохраняем данные о сотруднике
        string key = currentEmployee.lastName + " " + currentEmployee.firstName + " " + currentEmployee.middleName;
        employees[key] = currentEmployee;

        // Очищаем данные для следующего сотрудника
        currentEmployee = Employee();
    }

    file.close();
}

// Функция для подсчета стоимости всех договоров сотрудника
int totalCostOfContracts(const Employee& employee)
{
    int totalCost = 0;
    for (const auto& contract : employee.contracts)
    {
        totalCost += contract.cost;
    }
    return totalCost;
}

// Функция для получения списка договоров сотрудника
vector<Contract> contractsOfWork(const Employee& employee)
{
    return employee.contracts;
}

// Вспомогательная функция для конвертации строки в дату
std::tm parseDate(const std::string& date)
{
    std::tm tm = {};
    std::istringstream ss(date);
    ss >> std::get_time(&tm, "%d.%m.%Y");
    return tm;
}

// Функция для нахождения самого продолжительного договора сотрудника
Contract longestContract(const Employee& employee)
{
    auto longest = std::max_element(employee.contracts.begin(), employee.contracts.end(),
        [](const Contract& a, const Contract& b)
        {
            std::tm startA = parseDate(a.startDate);
            std::tm endA = parseDate(a.endDate);
            std::tm startB = parseDate(b.startDate);
            std::tm endB = parseDate(b.endDate);

            auto startA_tp = std::chrono::system_clock::from_time_t(std::mktime(&startA));
            auto endA_tp = std::chrono::system_clock::from_time_t(std::mktime(&endA));
            auto startB_tp = std::chrono::system_clock::from_time_t(std::mktime(&startB));
            auto endB_tp = std::chrono::system_clock::from_time_t(std::mktime(&endB));

            auto durationA = endA_tp - startA_tp;
            auto durationB = endB_tp - startB_tp;

            return durationA < durationB;
        });
    std::string contractNumber = longest->number;
    contractNumber.replace(0, 3, "contract №");
    std::cout << "Longest contract: " << contractNumber << std::endl;
    return *longest;
}

// Функция для нахождения самого дорогого договора сотрудника
Contract mostExpensiveContract(const Employee& employee)
{
    auto mostExpensive = max_element(employee.contracts.begin(), employee.contracts.end(),
        [](const Contract& a, const Contract& b)
        {
            return a.cost < b.cost;
        });
    // Заменяем "тДЦ" на "contract №" в номере самого дорогого договора
    string contractNumber = mostExpensive->number;
    contractNumber.replace(0, 3, "contract №");
    cout << "Most expensive contract: " << contractNumber << endl;
    return *mostExpensive;
}

// Функция для удаления информации о сотруднике
void removeEmployee(const string& lastName, const string& firstName, const string& middleName)
{
    string key = lastName + " " + firstName + " " + middleName;
    employees.erase(key);
}

void printEmployees() {
    for (const auto& pair : employees) {
        cout << pair.first << endl;
    }
}

int main()
{
    system("color F0");
    // Пример чтения данных из файла
    parseAndStoreData("data.txt");

    // Вывод содержимого employees после чтения данных из файла
    cout << "After parsing data:" << endl;
    printEmployees();
    cout << endl;

    // Выбираем сотрудника (например, по его фамилии, имени и отчеству)
    string lastName = "Ivanov";
    string firstName = "Ivan";
    string middleName = "Ivanovich";
    string key = lastName + " " + firstName + " " + middleName;

    // Проверяем, есть ли такой сотрудник
    if (employees.find(key) != employees.end())
    {
        Employee selectedEmployee = employees[key];

        // Пример использования функций только для выбранного сотрудника
        cout << "Employee: " << key << endl;
        cout << "Total cost of contracts: " << totalCostOfContracts(selectedEmployee) << endl;

        cout << "Contracts:" << endl;
        for (const auto& contract : contractsOfWork(selectedEmployee))
        {
            // Заменяем "тДЦ" на "contract №"
            string contractNumber = contract.number;
            contractNumber.replace(0, 3, "contract №");
            cout << contractNumber << " - " << contract.cost << endl;
        }

        longestContract(selectedEmployee);
        mostExpensiveContract(selectedEmployee);

        cout << endl;
    }
    else
    {
        cout << "Employee not found." << endl;
    }

    cout << "After remove:" << endl;
    removeEmployee("Ivanov", "Ivan", "Ivanovich");
    printEmployees();
    return 0;
}
