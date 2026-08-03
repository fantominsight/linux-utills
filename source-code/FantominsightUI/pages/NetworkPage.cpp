// Страница «Проверка сайта»: проверка доступности хоста через ping
#include "NetworkPage.h"

#include <QLabel>
#include <QLineEdit>
#include <QPalette>
#include <QPushButton>
#include <QRegularExpression>
#include <QVBoxLayout>

namespace {

// Собранная из вывода ping статистика
struct PingStats {
    int sent = 0;            // отправлено пакетов
    int received = 0;        // получено пакетов
    int loss = -1;           // процент потерь (-1 = не определено)
    double avg = -1.0;       // средний RTT в мс (-1 = не определено)
    bool summaryFound = false;  // нашлась ли строка со сводкой
};

// Разбирает вывод команды ping и заполняет структуру PingStats
PingStats parsePingOutput(const QString& output) {
    PingStats stats;

    // Сводная строка: "10 packets transmitted, 10 received, 0% packet loss"
    const QRegularExpression summaryRe(
        QStringLiteral(R"(^\s*(\d+)\s+packets transmitted,\s*(\d+)\s+received,\s*(\d+)%\s+packet loss)"),
        QRegularExpression::MultilineOption);
    // Строка со временами: "rtt min/avg/max/mdev = ..."
    const QRegularExpression rttRe(QStringLiteral(
        R"(rtt\s+min/avg/max/mdev\s*=\s*(\d+(?:\.\d+)?)\s*/\s*(\d+(?:\.\d+)?)\s*/\s*(\d+(?:\.\d+)?)\s*/\s*(\d+(?:\.\d+)?)\s*ms)"));

    const auto summary = summaryRe.match(output);
    if (summary.hasMatch()) {  // извлекаем числа из сводки
        stats.sent = summary.captured(1).toInt();
        stats.received = summary.captured(2).toInt();
        stats.loss = summary.captured(3).toInt();
        stats.summaryFound = true;
    }

    const auto rtt = rttRe.match(output);
    if (rtt.hasMatch()) {
        stats.avg = rtt.captured(2).toDouble();  // среднее время ответа
    }

    return stats;
}

// HTML-разметка для сообщения об ошибке (красным цветом)
QString errorHtml(const QString& message) {
    return QStringLiteral("<span style='color:#c62828; font-size:12pt;'>%1</span>")
        .arg(message.toHtmlEscaped());
}

// Формирует HTML-результат: статус (доступен/недоступен) и таблицу со статистикой
QString statusHtml(const PingStats& stats, bool reachable) {
    const QString statusColor = reachable ? QStringLiteral("#2e7d32") : QStringLiteral("#c62828");
    const QString statusText = reachable ? QStringLiteral("🟢 Доступен")
                                         : QStringLiteral("🔴 Недоступен");

    // Значения «—», если соответствующие данные не удалось извлечь
    const QString avg = stats.avg >= 0.0 ? QString::number(stats.avg, 'f', 2) : QStringLiteral("—");
    const QString sent = stats.summaryFound ? QString::number(stats.sent) : QStringLiteral("—");
    const QString received = stats.summaryFound ? QString::number(stats.received) : QStringLiteral("—");
    const QString loss = stats.loss >= 0 ? QString::number(stats.loss) : QStringLiteral("—");

    return QStringLiteral(
               "<div style='font-size:17pt; font-weight:600; color:%1;'>%2</div>"
               "<table style='margin-top:12px; font-size:11.5pt; color:#3c4043;"
               "border-collapse:collapse;'>"
               "<tr><td style='padding:4px 24px 4px 0;'>Средний ping:</td><td>%3 ms</td></tr>"
               "<tr><td style='padding:4px 24px 4px 0;'>Отправлено пакетов:</td><td>%4</td></tr>"
               "<tr><td style='padding:4px 24px 4px 0;'>Получено пакетов:</td><td>%5</td></tr>"
               "<tr><td style='padding:4px 24px 4px 0;'>Потеря пакетов:</td><td>%6%</td></tr>"
               "</table>")
        .arg(statusColor, statusText, avg, sent, received, loss);
}

}  // namespace

// Конструктор: собираем интерфейс и настраиваем процесс ping
NetworkPage::NetworkPage(QWidget* parent)
    : Page(parent) {
    // Корневой вертикальный лейаут
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(16);

    auto* title = new QLabel(QStringLiteral("Проверка доступности сайта"), this);
    title->setObjectName(QStringLiteral("pageTitle"));
    root->addWidget(title);

    auto* description = new QLabel(
        QStringLiteral("Введите IP-адрес или доменное имя сайта. "
                       "Будет отправлено 10 ping-запросов, результат появится ниже."),
        this);
    description->setObjectName(QStringLiteral("description"));
    description->setWordWrap(true);
    root->addWidget(description);

    // Поле ввода адреса с кнопкой очистки
    m_addressEdit = new QLineEdit(this);
    m_addressEdit->setObjectName(QStringLiteral("addressEdit"));
    m_addressEdit->setPlaceholderText(QStringLiteral("Введите IP или домен"));
    m_addressEdit->setClearButtonEnabled(true);

    // Настраиваем цвет текста-подсказки отдельно (он был нечитаемым со стилем по умолчанию)
    QPalette palette = m_addressEdit->palette();
    palette.setColor(QPalette::PlaceholderText, QColor(QStringLiteral("#9aa0a6")));
    m_addressEdit->setPalette(palette);
    root->addWidget(m_addressEdit);

    m_checkButton = new QPushButton(QStringLiteral("Проверить"), this);
    m_checkButton->setObjectName(QStringLiteral("primaryButton"));
    root->addWidget(m_checkButton, 0, Qt::AlignLeft);

    // Метка результата — принимает HTML (жирный статус, таблица)
    m_resultLabel = new QLabel(QStringLiteral("Здесь будет показан результат."), this);
    m_resultLabel->setObjectName(QStringLiteral("resultLabel"));
    m_resultLabel->setTextFormat(Qt::RichText);
    m_resultLabel->setWordWrap(true);
    m_resultLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    root->addWidget(m_resultLabel, 1);

    connect(m_checkButton, &QPushButton::clicked, this, &NetworkPage::onCheckClicked);

    // Процесс ping: объединяем stdout и stderr в один поток
    m_pingProcess.setProcessChannelMode(QProcess::MergedChannels);
    connect(&m_pingProcess, &QProcess::finished, this, &NetworkPage::onPingFinished);
    connect(&m_pingProcess, &QProcess::errorOccurred, this, &NetworkPage::onProcessError);
}

// Слот «Проверить»: запускаем ping с 10 пакетами
void NetworkPage::onCheckClicked() {
    if (m_pingProcess.state() != QProcess::NotRunning) {  // не запускаем поверх идущего ping
        return;
    }

    const QString address = m_addressEdit->text().trimmed();
    if (address.isEmpty()) {
        m_resultLabel->setText(errorHtml(QStringLiteral("Введите IP-адрес или домен.")));
        return;
    }

    setBusy(true);  // блокируем ввод на время проверки
    m_resultLabel->setText(QStringLiteral("⏳ Проверка... выполняется 10 ping-запросов"));

    // Запускаем системную команду: ping -c 10 <address>
    m_pingProcess.start(QStringLiteral("ping"),
                        {QStringLiteral("-c"), QStringLiteral("10"), address});
}

// Слот: ошибка запуска/работы процесса ping — запоминаем факт ошибки
void NetworkPage::onProcessError(QProcess::ProcessError /*error*/) {
    m_pingFailed = true;
}

// Слот: ping завершился — разбираем вывод и показываем результат
void NetworkPage::onPingFinished(int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/) {
    const QString output = QString::fromUtf8(m_pingProcess.readAllStandardOutput());
    setBusy(false);

    if (m_pingFailed) {  // если была ошибка запуска — сообщаем и выходим
        m_pingFailed = false;
        m_resultLabel->setText(errorHtml(QStringLiteral("Не удалось выполнить ping.")));
        return;
    }

    // Хост считается доступным, если в выводе есть сводка и получено хотя бы 1 пакет
    const PingStats stats = parsePingOutput(output);
    const bool reachable = stats.summaryFound && stats.received > 0;
    m_resultLabel->setText(statusHtml(stats, reachable));
}

// Блокирует/разблокирует поле ввода и кнопку во время проверки
void NetworkPage::setBusy(bool busy) {
    m_addressEdit->setEnabled(!busy);
    m_checkButton->setEnabled(!busy);
}
