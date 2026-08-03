/*
    Класс CommandPage: запуск внешних команд и отображение их вывода в окне приложения.
    Он наследуется от класса Page, который в свою очередь наследуется от QWidget.
    В конструкторе создаётся интерфейс: заголовок, описание, поле ввода,
    кнопка запуска и окно вывода. При нажатии кнопки запускается внешняя
    программа (см. program()/arguments() в наследниках), а её вывод
    в реальном времени дописывается в окно вывода.
*/
#include "CommandPage.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTextCursor>
#include <QVBoxLayout>

// Конструктор: собираем интерфейс и настраиваем объект QProcess
CommandPage::CommandPage(const QString& title, const QString& description,
                         const QString& placeholder, QWidget* parent)
    : Page(parent), m_title(title) {
    // Корневой вертикальный лейаут
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(16);

    // Заголовок страницы
    auto* titleLabel = new QLabel(title, this);
    titleLabel->setObjectName(QStringLiteral("pageTitle"));
    root->addWidget(titleLabel);

    // Описание страницы
    auto* descLabel = new QLabel(description, this);
    descLabel->setObjectName(QStringLiteral("description"));
    descLabel->setWordWrap(true);
    root->addWidget(descLabel);

    // Ряд: поле ввода + кнопка запуска
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

    // Окно вывода (только для чтения, ограничено 2000 строк истории)
    m_output = new QPlainTextEdit(this);
    m_output->setObjectName(QStringLiteral("output"));
    m_output->setReadOnly(true);
    m_output->setMaximumBlockCount(2000);
    root->addWidget(m_output, 1);

    // Клик по кнопке запускает команду
    connect(m_runButton, &QPushButton::clicked, this, &CommandPage::onRun);

    // Настройка QProcess: объединяем stdout и stderr в один поток,
    // чтобы вывод и ошибки печатались в едином порядке
    m_process.setProcessChannelMode(QProcess::MergedChannels);
    connect(&m_process, &QProcess::readyReadStandardOutput, this, &CommandPage::onReadyRead);
    connect(&m_process, &QProcess::finished, this, &CommandPage::onFinished);
    connect(&m_process, &QProcess::errorOccurred, this, &CommandPage::onError);
}

// Слот «Запустить»: проверяем ввод и стартуем внешний процесс
void CommandPage::onRun() {
    if (m_process.state() != QProcess::NotRunning) {  // не запускаем, пока идёт прошлый процесс
        return;
    }

    const QString input = m_input->text().trimmed();
    if (input.isEmpty()) {  // пустой адрес
        appendOutput(QStringLiteral("⚠ Укажите адрес."));
        return;
    }

    const QString program = this->program();
    if (program.isEmpty()) {  // исполняемый файл не найден
        appendOutput(QStringLiteral("⚠ Исполняемый файл не найден."));
        return;
    }

    m_output->clear();   // чистим окно вывода
    setBusy(true);       // блокируем ввод, пока идёт процесс
    // Печатаем командную строку в стиле "$ program аргументы"
    appendOutput(QStringLiteral("$ %1 %2").arg(program, arguments(input).join(QLatin1Char(' '))));

    m_process.start(program, arguments(input));  // запускаем процесс
}

// Слот: пришли новые данные из stdout процесса — выводим их в окно
void CommandPage::onReadyRead() {
    appendOutput(QString::fromLocal8Bit(m_process.readAllStandardOutput()));
}

// Слот: процесс завершился — сообщаем код возврата и разблокируем ввод
void CommandPage::onFinished(int exitCode, QProcess::ExitStatus /*exitStatus*/) {
    setBusy(false);
    appendOutput(QStringLiteral("\n— Завершено (код %1) —").arg(exitCode));
}

// Слот: ошибка процесса (не удалось запустить, обрыв и т.п.)
void CommandPage::onError(QProcess::ProcessError /*error*/) {
    if (m_process.state() == QProcess::NotRunning) {  // если процесс уже остановлен — разблокируем
        setBusy(false);
    }
    appendOutput(QStringLiteral("⚠ %1").arg(m_process.errorString()));
}

// Дописывает текст в окно вывода и прокручивает его вниз к последней строке
void CommandPage::appendOutput(const QString& text) {
    m_output->appendPlainText(text);
    m_output->moveCursor(QTextCursor::End);
}

// Блокирует/разблокирует поле ввода и кнопку во время работы процесса
void CommandPage::setBusy(bool busy) {
    m_input->setEnabled(!busy);
    m_runButton->setEnabled(!busy);
}
