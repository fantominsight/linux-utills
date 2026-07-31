#pragma once

#include "Page.h"

class QPlainTextEdit;
class QPushButton;

class Base64Page final : public Page {
    Q_OBJECT
public:
    explicit Base64Page(QWidget* parent = nullptr);

    QString title() const override { return QStringLiteral("Base64"); }

private slots:
    void onEncode();
    void onDecode();

private:
    QPlainTextEdit* m_input = nullptr;
    QPlainTextEdit* m_output = nullptr;
    QPushButton* m_encodeButton = nullptr;
    QPushButton* m_decodeButton = nullptr;
};
