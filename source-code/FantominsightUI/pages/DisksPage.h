#pragma once

#include "Page.h"

// Предварительные объявления классов Qt
class QPushButton;
class QTableWidget;

/*
    Страница «Диски».
    Показывает таблицу смонтированных дисковых устройств:
    устройство, точка монтирования, файловая система, объёмы
    и процент занятости. Данные обновляются по кнопке «Обновить».
*/
class DisksPage final : public Page {
    Q_OBJECT
public:
    explicit DisksPage(QWidget* parent = nullptr);

    QString title() const override { return QStringLiteral("Диски"); }

private slots:
    void refresh();  // перечитывает список дисков и заполняет таблицу

private:
    QTableWidget* m_table = nullptr;        // таблица с дисками
    QPushButton* m_refreshButton = nullptr; // кнопка «Обновить»
};
