#pragma once

#include <QTimer>

#include "Page.h"

class QLabel;
class QProgressBar;

class RamPage final : public Page {
    Q_OBJECT
public:
    explicit RamPage(QWidget* parent = nullptr);

    QString title() const override { return QStringLiteral("RAM"); }

private slots:
    void refresh();

private:
    struct MemInfo {
        quint64 total = 0;
        quint64 available = 0;
        quint64 swapTotal = 0;
        quint64 swapFree = 0;
        bool valid = false;
    };
    static MemInfo readMemInfo();

    QLabel* m_usedLabel = nullptr;
    QLabel* m_availableLabel = nullptr;
    QLabel* m_swapLabel = nullptr;
    QProgressBar* m_bar = nullptr;
    QTimer m_timer;
};
