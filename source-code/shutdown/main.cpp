#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>

int main() {
    std::cout << "[+] Запуск таймера выключения..." << std::endl;

    // Скрипт, который будет выполняться ВНУТРИ нового окна терминала
    std::string bashScript = 
        "bash -c '"
        "echo \"=========================================\"; "
        "echo \"  ВНИМАНИЕ! ПК ВЫКЛЮЧИТСЯ ЧЕРЕЗ 30 СЕКУНД! \"; "
        "echo \"  Чтобы отменить — просто закройте это окно! \"; "
        "echo \"=========================================\"; "
        "for i in {30..1}; do "
        "   echo -ne \"\\rОсталось времени: $i сек...\"; "
        "   sleep 1; "
        "done; "
        "echo -e \"\\n[!] Время вышло! Выключаем ПК...\"; "
        "poweroff"
        "'";

    // Команда для открытия нового окна GNOME Terminal с нашим скриптом
    std::string openTerminalCmd = "gnome-terminal -- " + bashScript;

    // Открываем новое окно терминала
    int result = system(openTerminalCmd.c_str());

    if (result != 0) {
        std::cerr << "[-] Ошибка: Не удалось открыть окно терминала!" << std::endl;
        return 1;
    }

    std::cout << "[+] Окно таймера успешно открыто." << std::endl;
    return 0;
}
