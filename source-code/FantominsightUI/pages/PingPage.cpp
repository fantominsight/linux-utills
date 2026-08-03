// Страница «Ping»: отправка ICMP-запросов к указанному адресу
#include "PingPage.h"

#include <QStandardPaths>

// Конструктор: передаём в CommandPage название, описание и подсказку поля ввода
PingPage::PingPage(QWidget* parent)
    : CommandPage(QStringLiteral("Ping"),
                  QStringLiteral("Отправка ICMP-запросов к указанному адресу. "
                                 "Вывод обновляется в реальном времени."),
                  QStringLiteral("Введите IP или домен"), parent) {}

// Ищем исполняемый файл ping в системном PATH
QString PingPage::program() const {
    return QStandardPaths::findExecutable(QStringLiteral("ping"));
}

// Аргументы: 10 пакетов (-c 10) + сам адрес
QStringList PingPage::arguments(const QString& input) const {
    return {QStringLiteral("-c"), QStringLiteral("10"), input};
}
