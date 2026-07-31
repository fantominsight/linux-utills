#pragma once

#include "Page.h"

class QPushButton;
class QTableWidget;

class DisksPage final : public Page {
    Q_OBJECT
public:
    explicit DisksPage(QWidget* parent = nullptr);

    QString title() const override { return QStringLiteral("Диски"); }

private slots:
    void refresh();

private:
    QTableWidget* m_table = nullptr;
    QPushButton* m_refreshButton = nullptr;
};
