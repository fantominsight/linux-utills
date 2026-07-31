#include "CommandPage.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTextCursor>
#include <QVBoxLayout>

CommandPage::CommandPage(const QString& title, const QString& description,
                         const QString& placeholder, QWidget* parent)
    : Page(parent), m_title(title) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(16);

    auto* titleLabel = new QLabel(title, this);
    titleLabel->setObjectName(QStringLiteral("pageTitle"));
    root->addWidget(titleLabel);

    auto* descLabel = new QLabel(description, this);
    descLabel->setObjectName(QStringLiteral("description"));
    descLabel->setWordWrap(true);
    root->addWidget(descLabel);

    auto* row = new QHBoxLayout;
    row->setSpacing(10);

    m_input = new QLineEdit(this);
    m_input->setObjectName(QStringLiteral("input"));
    m_input->setPlaceholderText(placeholder);
    row->addWidget(m_input, 1);

    m_runButton = new QPushButton(QStringLiteral("Запустить"), this);
    m_runButton->setObjectName(QStringLiteral("primaryButton"));
    row->addWidget(m_runButton);
    root->addLayout(row);

    m_output = new QPlainTextEdit(this);
    m_output->setObjectName(QStringLiteral("output"));
    m_output->setReadOnly(true);
    m_output->setMaximumBlockCount(2000);
    root->addWidget(m_output, 1);

    connect(m_runButton, &QPushButton::clicked, this, &CommandPage::onRun);

    m_process.setProcessChannelMode(QProcess::MergedChannels);
    connect(&m_process, &QProcess::readyReadStandardOutput, this, &CommandPage::onReadyRead);
    connect(&m_process, &QProcess::finished, this, &CommandPage::onFinished);
    connect(&m_process, &QProcess::errorOccurred, this, &CommandPage::onError);
}

void CommandPage::onRun() {
    if (m_process.state() != QProcess::NotRunning) {
        return;
    }

    const QString input = m_input->text().trimmed();
    if (input.isEmpty()) {
        appendOutput(QStringLiteral("⚠ Укажите адрес."));
        return;
    }

    const QString program = this->program();
    if (program.isEmpty()) {
        appendOutput(QStringLiteral("⚠ Исполняемый файл не найден."));
        return;
    }

    m_output->clear();
    setBusy(true);
    appendOutput(QStringLiteral("$ %1 %2").arg(program, arguments(input).join(QLatin1Char(' '))));

    m_process.start(program, arguments(input));
}

void CommandPage::onReadyRead() {
    appendOutput(QString::fromLocal8Bit(m_process.readAllStandardOutput()));
}

void CommandPage::onFinished(int exitCode, QProcess::ExitStatus /*exitStatus*/) {
    setBusy(false);
    appendOutput(QStringLiteral("\n— Завершено (код %1) —").arg(exitCode));
}

void CommandPage::onError(QProcess::ProcessError /*error*/) {
    if (m_process.state() == QProcess::NotRunning) {
        setBusy(false);
    }
    appendOutput(QStringLiteral("⚠ %1").arg(m_process.errorString()));
}

void CommandPage::appendOutput(const QString& text) {
    m_output->appendPlainText(text);
    m_output->moveCursor(QTextCursor::End);
}

void CommandPage::setBusy(bool busy) {
    m_input->setEnabled(!busy);
    m_runButton->setEnabled(!busy);
}
