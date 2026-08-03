#pragma once

#include "Page.h"

// Предварительные объявления классов Qt
class QPlainTextEdit;
class QPushButton;

/*
    Страница «Base64».
    Позволяет закодировать текст в Base64 и декодировать его обратно.
    Наследуется от Page, поэтому автоматически попадает в навигацию
    главного окна.
*/
class Base64Page final : public Page {
    Q_OBJECT
public:
    explicit Base64Page(QWidget* parent = nullptr);

    QString title() const override { return QStringLiteral("Base64"); }

private slots:
    void onEncode();  // кодирование текста в Base64
    void onDecode();  // декодирование Base64 в текст

private:
    QPlainTextEdit* m_input = nullptr;   // поле ввода исходных данных
    QPlainTextEdit* m_output = nullptr;  // поле вывода результата
    QPushButton* m_encodeButton = nullptr;  // кнопка «Кодировать»
    QPushButton* m_decodeButton = nullptr;  // кнопка «Декодировать»
};
