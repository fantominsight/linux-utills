#pragma once

#include "CommandPage.h"

/*
    Страница «Трассировка».
    Показывает маршрут следования пакетов до указанного адреса.
    Использует утилиту traceroute, а если её нет — tracepath.
    Реализует только program() и arguments() — интерфейс наследуется
    от CommandPage.
*/
class TracePage final : public CommandPage {
    Q_OBJECT
public:
    explicit TracePage(QWidget* parent = nullptr);

protected:
    QString program() const override;                 // путь к выбранной утилите трассировки
    QStringList arguments(const QString& input) const override;  // аргументы запуска

private:
    QString m_program;  // путь к найденной утилите (traceroute или tracepath)
};
