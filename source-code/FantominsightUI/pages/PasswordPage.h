#pragma once

#include "Page.h"

// Предварительные объявления классов Qt
class QCheckBox;
class QLineEdit;
class QPushButton;
class QSpinBox;

/*
    Страница «Генератор паролей».
    Генерирует случайный пароль заданной длины из выбранных наборов
    символов (строчные, заглавные буквы, цифры, спецсимволы) и позволяет
    скопировать его в буфер обмена.
*/
class PasswordPage final : public Page {
    Q_OBJECT
public:
    explicit PasswordPage(QWidget* parent = nullptr);

    QString title() const override { return QStringLiteral("Генератор паролей"); }

private slots:
    void onGenerate();  // генерация пароля
    void onCopy();      // копирование в буфер обмена

private:
    QSpinBox* m_lengthSpin = nullptr;   // выбор длины пароля
    QCheckBox* m_lowerBox = nullptr;    // включить a-z
    QCheckBox* m_upperBox = nullptr;    // включить A-Z
    QCheckBox* m_digitBox = nullptr;    // включить 0-9
    QCheckBox* m_symbolBox = nullptr;   // включить спецсимволы
    QPushButton* m_generateButton = nullptr;  // кнопка «Сгенерировать»
    QLineEdit* m_resultEdit = nullptr;  // поле с результатом
    QPushButton* m_copyButton = nullptr;    // кнопка «Копировать»
};
