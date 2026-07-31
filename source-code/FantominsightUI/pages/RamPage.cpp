#include "RamPage.h"

#include <QFile>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

namespace {

QString formatKb(quint64 kb) {
    return QString::number(double(kb) / 1048576.0, 'f', 2) + QStringLiteral(" ГБ");
}

}  // namespace

RamPage::RamPage(QWidget* parent)
    : Page(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(20);

    auto* titleLabel = new QLabel(QStringLiteral("Использование RAM"), this);
    titleLabel->setObjectName(QStringLiteral("pageTitle"));
    root->addWidget(titleLabel);

    m_usedLabel = new QLabel(QStringLiteral("—"), this);
    m_usedLabel->setAlignment(Qt::AlignCenter);
    m_usedLabel->setStyleSheet(QStringLiteral("font-size:30pt; font-weight:600; color:#1f232b;"));
    root->addWidget(m_usedLabel, 1);

    m_bar = new QProgressBar(this);
    m_bar->setRange(0, 100);
    m_bar->setTextVisible(false);
    m_bar->setFixedHeight(16);
    m_bar->setStyleSheet(QStringLiteral(
        "QProgressBar { background-color:#e4e8ee; border:none; border-radius:8px; }"
        "QProgressBar::chunk { background-color:#4b8cf8; border-radius:8px; }"));
    root->addWidget(m_bar);

    m_availableLabel = new QLabel(this);
    m_availableLabel->setObjectName(QStringLiteral("description"));
    m_availableLabel->setAlignment(Qt::AlignCenter);
    root->addWidget(m_availableLabel);

    m_swapLabel = new QLabel(this);
    m_swapLabel->setObjectName(QStringLiteral("description"));
    m_swapLabel->setAlignment(Qt::AlignCenter);
    root->addWidget(m_swapLabel);

    m_timer.setInterval(2000);
    connect(&m_timer, &QTimer::timeout, this, &RamPage::refresh);
    m_timer.start();

    refresh();
}

RamPage::MemInfo RamPage::readMemInfo() {
    MemInfo info;
    QFile file(QStringLiteral("/proc/meminfo"));
    if (!file.open(QIODevice::ReadOnly)) {
        return info;
    }
    auto takeValue = [&file](const char* key) -> quint64 {
        file.seek(0);
        while (true) {
            const QByteArray line = file.readLine();
            if (line.isEmpty()) {
                break;
            }
            if (line.startsWith(key)) {
                int tokenIndex = 0;
                for (const auto& token : line.split(' ')) {
                    if (token.isEmpty()) {
                        continue;
                    }
                    if (++tokenIndex == 2) {
                        return token.toULongLong();
                    }
                }
                return 0;
            }
        }
        return 0;
    };
    info.total = takeValue("MemTotal:");
    info.available = takeValue("MemAvailable:");
    info.swapTotal = takeValue("SwapTotal:");
    info.swapFree = takeValue("SwapFree:");
    info.valid = info.total > 0;
    return info;
}

void RamPage::refresh() {
    const MemInfo info = readMemInfo();
    if (!info.valid) {
        m_usedLabel->setText(QStringLiteral("Данные недоступны"));
        return;
    }

    const quint64 used = info.total > info.available ? info.total - info.available : 0;
    const int percent = qBound(0, int((100.0 * used) / info.total), 100);

    m_bar->setValue(percent);
    m_usedLabel->setText(QStringLiteral("%1% (%2 из %3)")
                             .arg(QString::number(percent),
                                  formatKb(used), formatKb(info.total)));
    m_availableLabel->setText(QStringLiteral("Свободно: %1").arg(formatKb(info.available)));
    if (info.swapTotal > 0) {
        const quint64 swapUsed = info.swapTotal > info.swapFree ? info.swapTotal - info.swapFree : 0;
        m_swapLabel->setText(QStringLiteral("Swap: занято %1 из %2")
                                 .arg(formatKb(swapUsed), formatKb(info.swapTotal)));
    } else {
        m_swapLabel->setText(QStringLiteral("Swap не настроен"));
    }
}
