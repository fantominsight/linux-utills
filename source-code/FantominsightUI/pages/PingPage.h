#pragma once

#include "CommandPage.h"

/*
    Страница «Ping».
    Готовая страница-команда на базе CommandPage: отправляет 10 ICMP-запросов
    (ping) указанному адресу и показывает вывод в реальном времени.
    Реализует только program() и arguments() — весь интерфейс берётся у CommandPage.
*/
class PingPage final : public CommandPage {
    Q_OBJECT
public:
    explicit PingPage(QWidget* parent = nullptr);

protected:
    QString program() const override;                 // путь к исполняемому файлу ping
    QStringList arguments(const QString& input) const override;  // аргументы запуска
};
