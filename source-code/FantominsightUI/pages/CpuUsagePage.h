#pragma once

#include <QPair>
#include <QTimer>

#include "Page.h"

// Предварительные объявления классов Qt
class QLabel;
class QProgressBar;

/*
    Страница «Загрузка CPU».
    Раз в секунду читает счётчики времени из /proc/stat, считает
    загрузку процессора в процентах и отображает её большим числом,
    полосой прогресса и поясняющей надписью.
*/
class CpuUsagePage final : public Page {
    Q_OBJECT
public:
    explicit CpuUsagePage(QWidget* parent = nullptr);

    QString title() const override { return QStringLiteral("Загрузка CPU"); }

private slots:
    void refresh();  // периодическое обновление показаний

private:
    // Читает из /proc/stat пару (время простоя, общее время) за всё время
    static QPair<quint64, quint64> readCpuTimes();

    QLabel* m_valueLabel = nullptr;   // большое число с процентами
    QLabel* m_infoLabel = nullptr;    // подпись «Обновляется каждую секунду»
    QProgressBar* m_bar = nullptr;    // полоса загрузки
    QTimer m_timer;                   // таймер периодического обновления
    QPair<quint64, quint64> m_previous{0, 0};  // предыдущие показания (для расчёта разницы)
};
