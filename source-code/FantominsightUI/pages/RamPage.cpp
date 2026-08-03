// Страница «RAM»: мониторинг оперативной памяти через /proc/meminfo
#include "RamPage.h"

#include <QFile>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

namespace {

// Переводит килобайты в гигабайты с двумя знаками после запятой
QString formatKb(quint64 kb) {
    return QString::number(double(kb) / 1048576.0, 'f', 2) + QStringLiteral(" ГБ");
}

}  // namespace

// Конструктор: собираем интерфейс и запускаем таймер обновления
RamPage::RamPage(QWidget* parent)
    : Page(parent) {
    // Корневой вертикальный лейаут
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(20);

    auto* titleLabel = new QLabel(QStringLiteral("Использование RAM"), this);
    titleLabel->setObjectName(QStringLiteral("pageTitle"));
    root->addWidget(titleLabel);

    // Крупная надпись занятой памяти (по центру, растягивается)
    m_usedLabel = new QLabel(QStringLiteral("—"), this);
    m_usedLabel->setAlignment(Qt::AlignCenter);
    m_usedLabel->setStyleSheet(QStringLiteral("font-size:30pt; font-weight:600; color:#1f232b;"));
    root->addWidget(m_usedLabel, 1);

    // Полоса занятой памяти
    m_bar = new QProgressBar(this);
    m_bar->setRange(0, 100);
    m_bar->setTextVisible(false);
    m_bar->setFixedHeight(16);
    m_bar->setStyleSheet(QStringLiteral(
        "QProgressBar { background-color:#e4e8ee; border:none; border-radius:8px; }"
        "QProgressBar::chunk { background-color:#4b8cf8; border-radius:8px; }"));
    root->addWidget(m_bar);

    // Строка «Свободно: ...»
    m_availableLabel = new QLabel(this);
    m_availableLabel->setObjectName(QStringLiteral("description"));
    m_availableLabel->setAlignment(Qt::AlignCenter);
    root->addWidget(m_availableLabel);

    // Строка про swap
    m_swapLabel = new QLabel(this);
    m_swapLabel->setObjectName(QStringLiteral("description"));
    m_swapLabel->setAlignment(Qt::AlignCenter);
    root->addWidget(m_swapLabel);

    // Таймер на 2 секунды
    m_timer.setInterval(2000);
    connect(&m_timer, &QTimer::timeout, this, &RamPage::refresh);
    m_timer.start();

    refresh();  // показываем данные сразу при открытии страницы
}

/*
    Читает /proc/meminfo. Файл состоит из строк вида
    "MemTotal:        16371084 kB" — ищем строку по ключу и берём второе
    слово (число). Лямбда takeValue сбрасывает указатель файла в начало
    и ищет нужную строку.
*/
RamPage::MemInfo RamPage::readMemInfo() {
    MemInfo info;
    QFile file(QStringLiteral("/proc/meminfo"));
    if (!file.open(QIODevice::ReadOnly)) {
        return info;  // valid остаётся false — данные недоступны
    }
    auto takeValue = [&file](const char* key) -> quint64 {
        file.seek(0);  // перечитываем файл с начала
        while (true) {
            const QByteArray line = file.readLine();
            if (line.isEmpty()) {
                break;  // конец файла — ключ не найден
            }
            if (line.startsWith(key)) {  // строка с нужным ключом
                int tokenIndex = 0;
                for (const auto& token : line.split(' ')) {  // делим строку по пробелам
                    if (token.isEmpty()) {
                        continue;
                    }
                    if (++tokenIndex == 2) {  // второе слово — значение в КБ
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
    info.valid = info.total > 0;  // данные считаем валидными, если есть общий объём
    return info;
}

// Слот таймера: пересчитываем показания и обновляем виджеты
void RamPage::refresh() {
    const MemInfo info = readMemInfo();
    if (!info.valid) {
        m_usedLabel->setText(QStringLiteral("Данные недоступны"));
        return;
    }

    // Занято = всего − доступно (с защитой от отрицательных значений)
    const quint64 used = info.total > info.available ? info.total - info.available : 0;
    const int percent = qBound(0, int((100.0 * used) / info.total), 100);

    m_bar->setValue(percent);
    m_usedLabel->setText(QStringLiteral("%1% (%2 из %3)")
                             .arg(QString::number(percent),
                                  formatKb(used), formatKb(info.total)));
    m_availableLabel->setText(QStringLiteral("Свободно: %1").arg(formatKb(info.available)));
    if (info.swapTotal > 0) {  // swap настроен — показываем занято/всего
        const quint64 swapUsed = info.swapTotal > info.swapFree ? info.swapTotal - info.swapFree : 0;
        m_swapLabel->setText(QStringLiteral("Swap: занято %1 из %2")
                                 .arg(formatKb(swapUsed), formatKb(info.swapTotal)));
    } else {
        m_swapLabel->setText(QStringLiteral("Swap не настроен"));
    }
}
