/*
    Точка входа в приложение.
    Здесь создаётся объект приложения Qt, главное окно MainWindow,
    в которое добавляются все страницы-инструменты, сгруппированные
    по трём разделам боковой панели, после чего запускается
    цикл обработки событий (пока окно не закроют).
*/

#include <QApplication>  // класс QApplication — основа приложения Qt Widgets

#include "MainWindow.h"  // главное окно приложения

#include "pages/Base64Page.h"    // кодирование/декодирование Base64
#include "pages/CpuUsagePage.h"  // загрузка процессора
#include "pages/DisksPage.h"     // диски
#include "pages/DnsPage.h"       // DNS-запросы
#include "pages/HashPage.h"      // хеш файлов
#include "pages/NetworkPage.h"   // проверка доступности сайта (ping)
#include "pages/PasswordPage.h"  // генератор паролей
#include "pages/PingPage.h"      // утилита ping
#include "pages/PortPage.h"      // проверка портов
#include "pages/RamPage.h"       // использование оперативной памяти
#include "pages/SysInfoPage.h"   // информация о системе
#include "pages/TracePage.h"     // трассировка маршрута (traceroute/tracepath)

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);  // создаём приложение Qt (обрабатывает события и аргументы CLI)

    MainWindow window;  // главное окно с боковой навигацией и стеком страниц

    // Раздел «Сеть» — инструменты для работы с сетью
    window.addGroup(QStringLiteral("🌐 Сеть"));
    window.addPage<NetworkPage>();  // проверка доступности сайта
    window.addPage<PingPage>();     // ping
    window.addPage<DnsPage>();      // DNS-запросы
    window.addPage<TracePage>();    // трассировка маршрута
    window.addPage<PortPage>();     // проверка портов

    // Раздел «Система» — информация и мониторинг системы
    window.addGroup(QStringLiteral("💻 Система"));
    window.addPage<SysInfoPage>();  // информация о ПК
    window.addPage<CpuUsagePage>(); // загрузка CPU
    window.addPage<RamPage>();      // загрузка RAM
    window.addPage<DisksPage>();    // диски

    // Раздел «Инструменты» — утилиты
    window.addGroup(QStringLiteral("🔧 Инструменты"));
    window.addPage<HashPage>();    // хеш файлов
    window.addPage<PasswordPage>();// генератор паролей
    window.addPage<Base64Page>();  // Base64

    window.show();         // показываем главное окно
    return app.exec();     // запускаем цикл обработки событий; возвращает код выхода
}
