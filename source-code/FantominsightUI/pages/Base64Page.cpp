#include "Base64Page.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

Base64Page::Base64Page(QWidget* parent)
    : Page(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(16);

    auto* titleLabel = new QLabel(QStringLiteral("Base64"), this);
    titleLabel->setObjectName(QStringLiteral("pageTitle"));
    root->addWidget(titleLabel);

    auto* descLabel = new QLabel(
        QStringLiteral("Кодирование и декодирование текста в Base64."), this);
    descLabel->setObjectName(QStringLiteral("description"));
    descLabel->setWordWrap(true);
    root->addWidget(descLabel);

    m_input = new QPlainTextEdit(this);
    m_input->setObjectName(QStringLiteral("output"));
    m_input->setPlaceholderText(QStringLiteral("Введите текст или Base64-данные"));
    root->addWidget(m_input, 1);

    auto* row = new QHBoxLayout;
    row->addStretch(1);

    m_encodeButton = new QPushButton(QStringLiteral("Кодировать →"), this);
    m_encodeButton->setObjectName(QStringLiteral("primaryButton"));
    row->addWidget(m_encodeButton);

    m_decodeButton = new QPushButton(QStringLiteral("→ Декодировать"), this);
    m_decodeButton->setObjectName(QStringLiteral("primaryButton"));
    row->addWidget(m_decodeButton);
    root->addLayout(row);

    m_output = new QPlainTextEdit(this);
    m_output->setObjectName(QStringLiteral("output"));
    m_output->setReadOnly(true);
    m_output->setPlaceholderText(QStringLiteral("Результат"));
    root->addWidget(m_output, 1);

    connect(m_encodeButton, &QPushButton::clicked, this, &Base64Page::onEncode);
    connect(m_decodeButton, &QPushButton::clicked, this, &Base64Page::onDecode);
}

void Base64Page::onEncode() {
    const QByteArray data = m_input->toPlainText().toUtf8();
    m_output->setPlainText(QString::fromLatin1(data.toBase64()));
}

void Base64Page::onDecode() {
    const QByteArray encoded = m_input->toPlainText().toUtf8();
    if (encoded.isEmpty()) {
        m_output->setPlainText(QString());
        return;
    }
    const QByteArray decoded = QByteArray::fromBase64(encoded);
    QString text = QString::fromUtf8(decoded);
    if (text.contains(QChar(0xFFFD))) {
        m_output->setPlainText(QStringLiteral("⚠ Некорректные Base64-данные или данные не в UTF-8."));
        return;
    }
    m_output->setPlainText(text);
}
