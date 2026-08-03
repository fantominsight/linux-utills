#pragma once

#include <QTimer>

#include "Page.h"

// Предварительные объявления классов Qt
class QLabel;
class QProgressBar;

/*
    Страница «RAM».
    Раз в 2 секунды читает /proc/meminfo, вычисляет занятую оперативную
    память и отображает её крупным числом, полосой прогресса,
    а также отдельной строкой показывает свободную память и состояние Swap.
*/
class RamPage final : public Page {
    Q_OBJECT
public:
    explicit RamPage(QWidget* parent = nullptr);

    QString title() const override { return QStringLiteral("RAM"); }

private slots:
    void refresh();  // периодическое обновление показаний

private:
    // Разобранные данные из /proc/meminfo
    struct MemInfo {
        quint64 total = 0;     // всего ОЗУ (КБ)
        quint64 available = 0; // доступно (КБ)
        quint64 swapTotal = 0; // всего swap (КБ)
        quint64 swapFree = 0;  // свободно swap (КБ)
        bool valid = false;    // удалось ли прочитать данные
    };
    static MemInfo readMemInfo();  // читает /proc/meminfo

    QLabel* m_usedLabel = nullptr;      // крупная надпись «X% (N из M)»
    QLabel* m_availableLabel = nullptr; // строка «Свободно: ...»
    QLabel* m_swapLabel = nullptr;      // строка про swap
    QProgressBar* m_bar = nullptr;      // полоса занятой памяти
    QTimer m_timer;                     // таймер периодического обновления
};
