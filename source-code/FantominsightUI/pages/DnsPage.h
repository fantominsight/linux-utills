#pragma once

#include <QDnsLookup>

#include "Page.h"

class QComboBox;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;

class DnsPage final : public Page {
    Q_OBJECT
public:
    explicit DnsPage(QWidget* parent = nullptr);

    QString title() const override { return QStringLiteral("DNS"); }

private slots:
    void onQuery();
    void onFinished();

private:
    QDnsLookup::Type currentType() const;
    void append(const QString& text);
    void printRecords();

    QLineEdit* m_nameEdit = nullptr;
    QComboBox* m_typeBox = nullptr;
    QPushButton* m_queryButton = nullptr;
    QPlainTextEdit* m_output = nullptr;
    QDnsLookup* m_lookup = nullptr;
};
