#pragma once

#include <QPair>
#include <QTimer>

#include "Page.h"

class QLabel;
class QProgressBar;

class CpuUsagePage final : public Page {
    Q_OBJECT
public:
    explicit CpuUsagePage(QWidget* parent = nullptr);

    QString title() const override { return QStringLiteral("Загрузка CPU"); }

private slots:
    void refresh();

private:
    static QPair<quint64, quint64> readCpuTimes();

    QLabel* m_valueLabel = nullptr;
    QLabel* m_infoLabel = nullptr;
    QProgressBar* m_bar = nullptr;
    QTimer m_timer;
    QPair<quint64, quint64> m_previous{0, 0};
};
