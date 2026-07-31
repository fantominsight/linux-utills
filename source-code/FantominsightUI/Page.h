#pragma once

#include <QString>
#include <QWidget>

class Page : public QWidget {
    Q_OBJECT
public:
    using QWidget::QWidget;

    virtual QString title() const = 0;
};
