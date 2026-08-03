#pragma once

#include "Page.h"

// Предварительное объявление класса Qt
class QTextEdit;

/*
    Страница «Информация о ПК».
    Собирает и показывает базовую информацию о системе: имя хоста,
    название продукта, ядро, архитектуру, модель CPU, число ядер
    и время работы с момента включения (uptime).
*/
class SysInfoPage final : public Page {
    Q_OBJECT
public:
    explicit SysInfoPage(QWidget* parent = nullptr);

    QString title() const override { return QStringLiteral("Информация о ПК"); }

private:
    void buildInfo();                                          // сбор и вывод информации
    void addField(const QString& name, const QString& value);  // добавление поля «Имя: значение»

    QTextEdit* m_text = nullptr;  // многострочное поле с информацией (только для чтения)
};
