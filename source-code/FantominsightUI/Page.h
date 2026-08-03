#pragma once

#include <QString>
#include <QWidget>

/*
    Базовый класс всех страниц-инструментов приложения.
    Каждая страница — это обычный QWidget (виджет), который дополнительно
    обязан вернуть своё название через title(). Это название используется
    главным окном для подписи кнопки в боковой панели навигации.
*/
class Page : public QWidget {
    Q_OBJECT
public:
    using QWidget::QWidget;  // наследуем все конструкторы QWidget

    virtual QString title() const = 0;  // чисто виртуальный метод: название страницы
};
