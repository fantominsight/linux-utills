#pragma once

#include <QProcess>

#include "Page.h"

class QLabel;
class QLineEdit;
class QPushButton;

class NetworkPage final : public Page {
    Q_OBJECT
public:
    explicit NetworkPage(QWidget* parent = nullptr);

    QString title() const override { return QStringLiteral("Проверка сайта"); }

private slots:
    void onCheckClicked();
    void onProcessError(QProcess::ProcessError error);
    void onPingFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void setBusy(bool busy);

    QLineEdit* m_addressEdit = nullptr;
    QPushButton* m_checkButton = nullptr;
    QLabel* m_resultLabel = nullptr;
    QProcess m_pingProcess;
    bool m_pingFailed = false;
};
