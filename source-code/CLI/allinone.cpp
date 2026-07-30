#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <sstream>

using namespace std;

int PromgramFind () {
    string find;
    cout << "Enter the program you want to find!" << endl;
    cout << "(Fantominsight_shell)#> ";
    cin >> find;
    cout << "Searching..." << "'" << find << "'" << "now" << endl;
    system("sleep 3");
    system(("which " + find).c_str());

    int result = system(("whereis " + find).c_str()); 
        if (result != 0) { 
    cout << "Error! I can not find this program..." << endl;
    return 0;
    }
        return 0;
}   

int FantomPing () {
        string url;

    cout << "======================================" << endl;
    cout << " Проверка доступности сайта или сервера!" << endl;
    cout << "======================================" << endl;

    cout << "Введите URL сайта или сервера и подождите 5-10 секунд!" << endl;
    cout << "Например fantominsight.com, 192.168.0.1" << endl;
    cout << "(Fantominsight)#: ";
    cin >> url;

    // Выполняем ping
    system(("ping -c 10 " + url + " > ping_result.txt").c_str());

    ifstream file("ping_result.txt");

    if (!file.is_open()) {
        cout << "Ошибка открытия файла!" << endl;
        return 1;
    }

    string line;
    bool reachable = false;
    double avgPing = -1;

    while (getline(file, line)) {

        // Проверяем количество потерянных пакетов
        if (line.find("0% packet loss") != string::npos ||
            line.find("received") != string::npos) {
            reachable = true;
        }

        // Ищем строку со статистикой RTT
        if (line.find("rtt") != string::npos) {

            size_t pos = line.find('=');

            if (pos != string::npos) {

                string stats = line.substr(pos + 2);

                // min/avg/max/mdev
                double min, max, mdev;
                char slash;

                stringstream ss(stats);
                ss >> min >> slash >> avgPing >> slash >> max >> slash >> mdev;
            }
        }
    }

    file.close();

    cout << "\n========== Результат ==========\n";

    if (!reachable) {
        cout << "Статус: 🔴 Недоступен" << endl;
    }
    else if (avgPing > 100) {
        cout << "Статус: 🟡 Доступен с задержкой" << endl;
        cout << "Средний пинг: " << avgPing << " ms" << endl;
    }
    else {
        cout << "Статус: 🟢 Доступен" << endl;
        cout << "Средний пинг: " << avgPing << " ms" << endl;
    }

    remove("ping_result.txt");

    return 0;
}

int pwd () {
    cout << "Wait..." << endl;
    system("sleep 2");
    system("pwd");
    return 0; 
}

int whoami () {
    cout << "Wait..." << endl;
    system("sleep 2");
    system("whoami");
    return 0; 
}

int reboot () {
    cout << "Wait..." << endl;
    system("sleep 20");
    system("reboot");
    return 0; 
}

int poweroff () {
    cout << "Wait..." << endl;
    system("sleep 2");
    system("poweroff");
    return 0; 
}

int main() {
    int choice;

    while (true) {
        cout << "\n========== FantomInsight Shell Utility ==========\n";
        cout << "1. Найти программу (which/whereis)\n";
        cout << "2. FantomPing\n";
        cout << "3. Показать текущую директорию (pwd)\n";
        cout << "4. Узнать текущего пользователя (whoami)\n";
        cout << "5. Перезагрузить компьютер(если поддерживается)\n";
        cout << "6. Выключить компьютер(если поддерживается)\n";
        cout << "0. Выход\n";
        cout << "===========================================\n";
        cout << "(Fantominsight_shell#>): ";

        cin >> choice;

        switch (choice) {
            case 1:
                PromgramFind();
                break;

            case 2:
                FantomPing();
                break;

            case 3:
                pwd();
                break;

            case 4:
                whoami();
                break;

            case 5:
                reboot();
                break;

            case 6:
                poweroff();
                break;

            case 0:
                cout << "До свидания!" << endl;
                return 0;

            default:
                cout << "Ошибка! Такого пункта нет." << endl;
        }

        cout << "\nНажмите Enter для возврата в меню...";
        cin.ignore();
        cin.get();

        system("clear");
    }

    return 0;
}