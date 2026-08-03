#pragma once

#include <QDnsLookup>

#include "Page.h"

// Предварительные объявления классов Qt
class QComboBox;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;

/*
    Страница «DNS».
    Отправляет DNS-запрос выбранного типа (A, AAAA, MX, TXT и т.д.)
    через системный резолвер (QDnsLookup) и выводит найденные записи.
*/
class DnsPage final : public Page {
    Q_OBJECT
public:
    explicit DnsPage(QWidget* parent = nullptr);

    QString title() const override { return QStringLiteral("DNS"); }

private slots:
    void onQuery();    // начало DNS-запроса
    void onFinished(); // завершение DNS-запроса

private:
    QDnsLookup::Type currentType() const;  // тип запроса, выбранный в комбобоксе
    void append(const QString& text);      // дописать строку в окно вывода
    void printRecords();                   // вывод записей в зависимости от типа

    QLineEdit* m_nameEdit = nullptr;    // поле ввода домена
    QComboBox* m_typeBox = nullptr;     // выбор типа записи
    QPushButton* m_queryButton = nullptr;  // кнопка «Запросить»
    QPlainTextEdit* m_output = nullptr;    // окно вывода результатов
    QDnsLookup* m_lookup = nullptr;        // объект DNS-запроса (создаётся на каждый запрос)
};
