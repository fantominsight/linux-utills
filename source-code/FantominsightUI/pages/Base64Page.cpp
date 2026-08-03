/*
    Страница «Base64»: кодирование и декодирование текста в Base64 и обратно.
    Наследуется от класса Page, который в свою очередь наследуется от QWidget.
    В конструкторе создаётся интерфейс: заголовок, описание, поле ввода,
    кнопки «Кодировать»/«Декодировать» и окно вывода результата.
*/

#include "Base64Page.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

// Конструктор: собираем виджеты страницы и связываем кнопки со слотами
Base64Page::Base64Page(QWidget* parent)
    : Page(parent) {
    // Корневой вертикальный лейаут с отступами от краёв и расстоянием между виджетами
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(16);

    // Заголовок страницы
    auto* titleLabel = new QLabel(QStringLiteral("Base64"), this);
    titleLabel->setObjectName(QStringLiteral("pageTitle"));
    root->addWidget(titleLabel);

    // Краткое описание назначения страницы
    auto* descLabel = new QLabel(
        QStringLiteral("Кодирование и декодирование текста в Base64."), this);
    descLabel->setObjectName(QStringLiteral("description"));
    descLabel->setWordWrap(true);  // перенос слов для длинных описаний
    root->addWidget(descLabel);

    // Поле ввода исходного текста (растягивается по вертикали)
    m_input = new QPlainTextEdit(this);
    m_input->setObjectName(QStringLiteral("output"));
    m_input->setPlaceholderText(QStringLiteral("Введите текст или Base64-данные"));
    root->addWidget(m_input, 1);

    // Горизонтальный ряд с кнопками (пружина слева прижимает их к правому краю)
    auto* row = new QHBoxLayout;
    row->addStretch(1);

    m_encodeButton = new QPushButton(QStringLiteral("Кодировать →"), this);
    m_encodeButton->setObjectName(QStringLiteral("primaryButton"));
    row->addWidget(m_encodeButton);

    m_decodeButton = new QPushButton(QStringLiteral("→ Декодировать"), this);
    m_decodeButton->setObjectName(QStringLiteral("primaryButton"));
    row->addWidget(m_decodeButton);
    root->addLayout(row);

    // Окно вывода результата (только для чтения)
    m_output = new QPlainTextEdit(this);
    m_output->setObjectName(QStringLiteral("output"));
    m_output->setReadOnly(true);
    m_output->setPlaceholderText(QStringLiteral("Результат"));
    root->addWidget(m_output, 1);

    // Связываем клики по кнопкам с соответствующими слотами
    connect(m_encodeButton, &QPushButton::clicked, this, &Base64Page::onEncode);
    connect(m_decodeButton, &QPushButton::clicked, this, &Base64Page::onDecode);
}

// Слот «Кодировать»: берём текст из поля ввода, переводим в UTF-8 и кодируем в Base64
void Base64Page::onEncode() {
    const QByteArray data = m_input->toPlainText().toUtf8();
    // toBase64() возвращает ASCII-данные, поэтому используем fromLatin1
    m_output->setPlainText(QString::fromLatin1(data.toBase64()));
}

// Слот «Декодировать»: переводим Base64 обратно в байты и в текст UTF-8
void Base64Page::onDecode() {
    const QByteArray encoded = m_input->toPlainText().toUtf8();
    if (encoded.isEmpty()) {  // пустой ввод — пустой вывод
        m_output->setPlainText(QString());
        return;
    }
    const QByteArray decoded = QByteArray::fromBase64(encoded);
    QString text = QString::fromUtf8(decoded);
    // Символ U+FFFD (�) появляется при невалидной UTF-8-последовательности.
    // Значит, данные — не Base64 или это не текст в кодировке UTF-8.
    if (text.contains(QChar(0xFFFD))) {
        m_output->setPlainText(QStringLiteral("⚠ Некорректные Base64-данные или данные не в UTF-8."));
        return;
    }
    m_output->setPlainText(text);
}
