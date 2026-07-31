#pragma once

#include "CommandPage.h"

class PingPage final : public CommandPage {
    Q_OBJECT
public:
    explicit PingPage(QWidget* parent = nullptr);

protected:
    QString program() const override;
    QStringList arguments(const QString& input) const override;
};
