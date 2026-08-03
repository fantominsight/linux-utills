#pragma once

#include <QCryptographicHash>

#include "Page.h"

// Предварительные объявления классов Qt
class QComboBox;
class QLineEdit;
class QPushButton;

/*
    Страница «Хеш файлов».
    Вычисляет контрольную сумму (хеш) выбранного файла одним из алгоритмов:
    MD5, SHA-1, SHA-224/256/384/512, SHA3-256/512. Файл читается
    поблочно, поэтому работает и с большими файлами.
*/
class HashPage final : public Page {
    Q_OBJECT
public:
    explicit HashPage(QWidget* parent = nullptr);

    QString title() const override { return QStringLiteral("Хеш файлов"); }

private slots:
    void browse();  // открыть диалог выбора файла
    void onHash();  // вычислить хеш

private:
    QCryptographicHash::Algorithm currentAlgorithm() const;  // алгоритм из комбобокса
    QString computeHash(const QString& path, QCryptographicHash::Algorithm algorithm);  // сам расчёт

    QLineEdit* m_pathEdit = nullptr;    // путь к файлу
    QPushButton* m_browseButton = nullptr;  // кнопка «Обзор»
    QComboBox* m_algoBox = nullptr;     // выбор алгоритма
    QPushButton* m_hashButton = nullptr;    // кнопка «Вычислить»
    QLineEdit* m_resultEdit = nullptr;  // поле с результатом (только для чтения)
};
