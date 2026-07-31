#pragma once

#include <QProcess>

#include "Page.h"

class QLineEdit;
class QPlainTextEdit;
class QPushButton;

class CommandPage : public Page {
    Q_OBJECT
public:
    explicit CommandPage(const QString& title, const QString& description,
                         const QString& placeholder, QWidget* parent = nullptr);

    QString title() const override { return m_title; }

protected:
    virtual QString program() const = 0;
    virtual QStringList arguments(const QString& input) const = 0;

    void appendOutput(const QString& text);
    void setBusy(bool busy);
    QPlainTextEdit* output() const { return m_output; }

private slots:
    void onRun();
    void onReadyRead();
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onError(QProcess::ProcessError error);

private:
    QString m_title;
    QLineEdit* m_input = nullptr;
    QPushButton* m_runButton = nullptr;
    QPlainTextEdit* m_output = nullptr;
    QProcess m_process;
};
