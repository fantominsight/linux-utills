#pragma once

#include "Page.h"

class QTextEdit;

class SysInfoPage final : public Page {
    Q_OBJECT
public:
    explicit SysInfoPage(QWidget* parent = nullptr);

    QString title() const override { return QStringLiteral("Информация о ПК"); }

private:
    void buildInfo();
    void addField(const QString& name, const QString& value);

    QTextEdit* m_text = nullptr;
};
