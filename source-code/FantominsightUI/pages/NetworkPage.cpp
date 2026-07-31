#include "NetworkPage.h"

#include <QLabel>
#include <QLineEdit>
#include <QPalette>
#include <QPushButton>
#include <QRegularExpression>
#include <QVBoxLayout>

namespace {

struct PingStats {
    int sent = 0;
    int received = 0;
    int loss = -1;
    double avg = -1.0;
    bool summaryFound = false;
};

PingStats parsePingOutput(const QString& output) {
    PingStats stats;

    const QRegularExpression summaryRe(
        QStringLiteral(R"(^\s*(\d+)\s+packets transmitted,\s*(\d+)\s+received,\s*(\d+)%\s+packet loss)"),
        QRegularExpression::MultilineOption);
    const QRegularExpression rttRe(QStringLiteral(
        R"(rtt\s+min/avg/max/mdev\s*=\s*(\d+(?:\.\d+)?)\s*/\s*(\d+(?:\.\d+)?)\s*/\s*(\d+(?:\.\d+)?)\s*/\s*(\d+(?:\.\d+)?)\s*ms)"));

    const auto summary = summaryRe.match(output);
    if (summary.hasMatch()) {
        stats.sent = summary.captured(1).toInt();
        stats.received = summary.captured(2).toInt();
        stats.loss = summary.captured(3).toInt();
        stats.summaryFound = true;
    }

    const auto rtt = rttRe.match(output);
    if (rtt.hasMatch()) {
        stats.avg = rtt.captured(2).toDouble();
    }

    return stats;
}

QString errorHtml(const QString& message) {
    return QStringLiteral("<span style='color:#c62828; font-size:12pt;'>%1</span>")
        .arg(message.toHtmlEscaped());
}

QString statusHtml(const PingStats& stats, bool reachable) {
    const QString statusColor = reachable ? QStringLiteral("#2e7d32") : QStringLiteral("#c62828");
    const QString statusText = reachable ? QStringLiteral("🟢 Доступен")
                                         : QStringLiteral("🔴 Недоступен");

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

NetworkPage::NetworkPage(QWidget* parent)
    : Page(parent) {
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

    m_addressEdit = new QLineEdit(this);
    m_addressEdit->setObjectName(QStringLiteral("addressEdit"));
    m_addressEdit->setPlaceholderText(QStringLiteral("Введите IP или домен"));
    m_addressEdit->setClearButtonEnabled(true);

    QPalette palette = m_addressEdit->palette();
    palette.setColor(QPalette::PlaceholderText, QColor(QStringLiteral("#9aa0a6")));
    m_addressEdit->setPalette(palette);
    root->addWidget(m_addressEdit);

    m_checkButton = new QPushButton(QStringLiteral("Проверить"), this);
    m_checkButton->setObjectName(QStringLiteral("primaryButton"));
    root->addWidget(m_checkButton, 0, Qt::AlignLeft);

    m_resultLabel = new QLabel(QStringLiteral("Здесь будет показан результат."), this);
    m_resultLabel->setObjectName(QStringLiteral("resultLabel"));
    m_resultLabel->setTextFormat(Qt::RichText);
    m_resultLabel->setWordWrap(true);
    m_resultLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    root->addWidget(m_resultLabel, 1);

    connect(m_checkButton, &QPushButton::clicked, this, &NetworkPage::onCheckClicked);

    m_pingProcess.setProcessChannelMode(QProcess::MergedChannels);
    connect(&m_pingProcess, &QProcess::finished, this, &NetworkPage::onPingFinished);
    connect(&m_pingProcess, &QProcess::errorOccurred, this, &NetworkPage::onProcessError);
}

void NetworkPage::onCheckClicked() {
    if (m_pingProcess.state() != QProcess::NotRunning) {
        return;
    }

    const QString address = m_addressEdit->text().trimmed();
    if (address.isEmpty()) {
        m_resultLabel->setText(errorHtml(QStringLiteral("Введите IP-адрес или домен.")));
        return;
    }

    setBusy(true);
    m_resultLabel->setText(QStringLiteral("⏳ Проверка... выполняется 10 ping-запросов"));

    m_pingProcess.start(QStringLiteral("ping"),
                        {QStringLiteral("-c"), QStringLiteral("10"), address});
}

void NetworkPage::onProcessError(QProcess::ProcessError /*error*/) {
    m_pingFailed = true;
}

void NetworkPage::onPingFinished(int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/) {
    const QString output = QString::fromUtf8(m_pingProcess.readAllStandardOutput());
    setBusy(false);

    if (m_pingFailed) {
        m_pingFailed = false;
        m_resultLabel->setText(errorHtml(QStringLiteral("Не удалось выполнить ping.")));
        return;
    }

    const PingStats stats = parsePingOutput(output);
    const bool reachable = stats.summaryFound && stats.received > 0;
    m_resultLabel->setText(statusHtml(stats, reachable));
}

void NetworkPage::setBusy(bool busy) {
    m_addressEdit->setEnabled(!busy);
    m_checkButton->setEnabled(!busy);
}
