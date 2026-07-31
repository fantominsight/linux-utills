#include "CpuUsagePage.h"

#include <QFile>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

CpuUsagePage::CpuUsagePage(QWidget* parent)
    : Page(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(20);

    auto* titleLabel = new QLabel(QStringLiteral("Загрузка CPU"), this);
    titleLabel->setObjectName(QStringLiteral("pageTitle"));
    root->addWidget(titleLabel);

    m_valueLabel = new QLabel(QStringLiteral("—"), this);
    m_valueLabel->setAlignment(Qt::AlignCenter);
    m_valueLabel->setStyleSheet(QStringLiteral("font-size:34pt; font-weight:600; color:#1f232b;"));
    root->addWidget(m_valueLabel, 1);

    m_bar = new QProgressBar(this);
    m_bar->setRange(0, 100);
    m_bar->setTextVisible(false);
    m_bar->setFixedHeight(16);
    m_bar->setStyleSheet(QStringLiteral(
        "QProgressBar { background-color:#e4e8ee; border:none; border-radius:8px; }"
        "QProgressBar::chunk { background-color:#4b8cf8; border-radius:8px; }"));
    root->addWidget(m_bar);

    m_infoLabel = new QLabel(this);
    m_infoLabel->setObjectName(QStringLiteral("description"));
    m_infoLabel->setAlignment(Qt::AlignCenter);
    root->addWidget(m_infoLabel);

    m_timer.setInterval(1000);
    connect(&m_timer, &QTimer::timeout, this, &CpuUsagePage::refresh);
    m_timer.start();

    m_previous = readCpuTimes();
}

QPair<quint64, quint64> CpuUsagePage::readCpuTimes() {
    QFile file(QStringLiteral("/proc/stat"));
    if (!file.open(QIODevice::ReadOnly)) {
        return {0, 0};
    }
    const QStringList parts =
        QString::fromLatin1(file.readLine()).trimmed().split(QLatin1Char(' '), Qt::SkipEmptyParts);
    if (parts.size() < 5 || parts.first() != QLatin1String("cpu")) {
        return {0, 0};
    }
    quint64 total = 0;
    quint64 idle = 0;
    for (int i = 1; i < parts.size() && i <= 8; ++i) {
        const quint64 value = parts[i].toULongLong();
        total += value;
        if (i == 4 || i == 5) {  // idle + iowait
            idle += value;
        }
    }
    return {idle, total};
}

void CpuUsagePage::refresh() {
    const auto current = readCpuTimes();
    double percent = 0.0;
    if (m_previous.second > 0 && current.second > m_previous.second) {
        const quint64 dTotal = current.second - m_previous.second;
        const quint64 dIdle = current.first - m_previous.first;
        percent = 100.0 * (1.0 - (static_cast<double>(dIdle) / static_cast<double>(dTotal)));
    }
    m_previous = current;

    const int rounded = qBound(0, int(percent + 0.5), 100);
    m_bar->setValue(rounded);
    m_valueLabel->setText(QStringLiteral("%1%").arg(QString::number(percent, 'f', 1)));
    m_infoLabel->setText(QStringLiteral("Обновляется каждую секунду"));
}
