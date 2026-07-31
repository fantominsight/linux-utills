#pragma once

#include "Page.h"

class QCheckBox;
class QLineEdit;
class QPushButton;
class QSpinBox;

class PasswordPage final : public Page {
    Q_OBJECT
public:
    explicit PasswordPage(QWidget* parent = nullptr);

    QString title() const override { return QStringLiteral("Генератор паролей"); }

private slots:
    void onGenerate();
    void onCopy();

private:
    QSpinBox* m_lengthSpin = nullptr;
    QCheckBox* m_lowerBox = nullptr;
    QCheckBox* m_upperBox = nullptr;
    QCheckBox* m_digitBox = nullptr;
    QCheckBox* m_symbolBox = nullptr;
    QPushButton* m_generateButton = nullptr;
    QLineEdit* m_resultEdit = nullptr;
    QPushButton* m_copyButton = nullptr;
};
