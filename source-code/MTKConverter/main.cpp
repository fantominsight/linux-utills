#include <iostream>
#include <string>  

using namespace std;

int main() {
    const double KmInMile = 1.609344;
    string input; 

    while (true) { 
        cout << "Укажите расстояние в милях (или exit для выхода): ";
        cin >> input;

        if (input == "exit") {
            cout << "Завершение работы программы..." << endl;
            break;
        }

        double distMile = stod(input); 

        double distKm = distMile * KmInMile;
        cout << "Расстояние в км: " << distKm << endl << endl;
    }

    return 0;
}
