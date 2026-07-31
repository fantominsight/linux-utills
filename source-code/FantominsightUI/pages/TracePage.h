#pragma once

#include "CommandPage.h"

class TracePage final : public CommandPage {
    Q_OBJECT
public:
    explicit TracePage(QWidget* parent = nullptr);

protected:
    QString program() const override;
    QStringList arguments(const QString& input) const override;

private:
    QString m_program;
};
