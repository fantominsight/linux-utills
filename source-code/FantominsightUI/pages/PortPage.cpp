#include "PortPage.h"

#include <QAbstractSocket>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTcpSocket>
#include <QTimer>
#include <QVBoxLayout>

PortPage::PortPage(QWidget* parent)
    : Page(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(16);

    auto* titleLabel = new QLabel(QStringLiteral("Проверка портов"), this);
    titleLabel->setObjectName(QStringLiteral("pageTitle"));
    root->addWidget(titleLabel);

    auto* descLabel = new QLabel(
        QStringLiteral("Проверка доступности TCP-порта на указанном хосте."), this);
    descLabel->setObjectName(QStringLiteral("description"));
    descLabel->setWordWrap(true);
    root->addWidget(descLabel);

    auto* row = new QHBoxLayout;
    row->setSpacing(10);

    m_hostEdit = new QLineEdit(this);
    m_hostEdit->setObjectName(QStringLiteral("input"));
    m_hostEdit->setPlaceholderText(QStringLiteral("Введите IP или домен"));
    row->addWidget(m_hostEdit, 1);

    m_portSpin = new QSpinBox(this);
    m_portSpin->setRange(1, 65535);
    m_portSpin->setValue(80);
    m_portSpin->setFixedWidth(110);
    row->addWidget(m_portSpin);

    m_checkButton = new QPushButton(QStringLiteral("Проверить"), this);
    m_checkButton->setObjectName(QStringLiteral("primaryButton"));
    row->addWidget(m_checkButton);
    root->addLayout(row);

    m_resultLabel = new QLabel(QStringLiteral("Здесь будет показан результат."), this);
    m_resultLabel->setObjectName(QStringLiteral("resultLabel"));
    m_resultLabel->setTextFormat(Qt::RichText);
    m_resultLabel->setWordWrap(true);
    m_resultLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    root->addWidget(m_resultLabel, 1);

    connect(m_checkButton, &QPushButton::clicked, this, &PortPage::onCheck);

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    m_timer->setInterval(4000);
    connect(m_timer, &QTimer::timeout, this, &PortPage::onTimeout);
}

void PortPage::onCheck() {
    const QString host = m_hostEdit->text().trimmed();
    if (host.isEmpty()) {
        m_resultLabel->setText(QStringLiteral("<span style='color:#c62828;'>Укажите хост.</span>"));
        return;
    }

    setBusy(true);
    m_resultLabel->setText(QStringLiteral("⏳ Проверка %1:%2 ...")
                               .arg(host, QString::number(m_portSpin->value())));

    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::connected, this, &PortPage::onConnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &PortPage::onSocketError);
    m_socket->connectToHost(host, quint16(m_portSpin->value()));
    m_timer->start();
}

void PortPage::onConnected() {
    if (!m_socket) {
        return;
    }
    m_timer->stop();
    finishCheck(QStringLiteral(
        "<span style='font-size:17pt; font-weight:600; color:#2e7d32;'>🟢 Порт открыт</span><br/>"
        "Соединение установлено."));
}

void PortPage::onSocketError(QAbstractSocket::SocketError /*error*/) {
    if (!m_socket) {
        return;
    }
    m_timer->stop();
    const QString reason = m_socket->errorString();
    QString note;
    if (reason.contains(QStringLiteral("refused"), Qt::CaseInsensitive)) {
        note = QStringLiteral("Соединение отклонено.");
    } else if (reason.contains(QStringLiteral("timeout"), Qt::CaseInsensitive) ||
               reason.contains(QStringLiteral("timed out"), Qt::CaseInsensitive)) {
        note = QStringLiteral("Время ожидания истекло.");
    } else {
        note = reason;
    }
    finishCheck(QStringLiteral(
                    "<span style='font-size:17pt; font-weight:600; color:#c62828;'>🔴 Порт закрыт "
                    "или недоступен</span><br/>%1")
                    .arg(note.toHtmlEscaped()));
}

void PortPage::onTimeout() {
    if (!m_socket) {
        return;
    }
    m_socket->abort();
    finishCheck(QStringLiteral(
        "<span style='font-size:17pt; font-weight:600; color:#c62828;'>🔴 Порт закрыт или "
        "недоступен</span><br/>Время ожидания ответа истекло."));
}

void PortPage::finishCheck(const QString& html) {
    setBusy(false);
    m_resultLabel->setText(html);
    if (m_socket) {
        m_socket->deleteLater();
        m_socket = nullptr;
    }
}

void PortPage::setBusy(bool busy) {
    m_hostEdit->setEnabled(!busy);
    m_portSpin->setEnabled(!busy);
    m_checkButton->setEnabled(!busy);
}
