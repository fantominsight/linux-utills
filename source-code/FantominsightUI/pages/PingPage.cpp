#include "PingPage.h"

#include <QStandardPaths>

PingPage::PingPage(QWidget* parent)
    : CommandPage(QStringLiteral("Ping"),
                  QStringLiteral("Отправка ICMP-запросов к указанному адресу. "
                                 "Вывод обновляется в реальном времени."),
                  QStringLiteral("Введите IP или домен"), parent) {}

QString PingPage::program() const {
    return QStandardPaths::findExecutable(QStringLiteral("ping"));
}

QStringList PingPage::arguments(const QString& input) const {
    return {QStringLiteral("-c"), QStringLiteral("10"), input};
}
