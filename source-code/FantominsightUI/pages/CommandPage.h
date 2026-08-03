#pragma once

#include <QProcess>

#include "Page.h"

// Предварительные объявления классов Qt
class QLineEdit;
class QPlainTextEdit;
class QPushButton;

/*
    Абстрактная страница «запуск внешней команды».
    Родительский класс для PingPage и TracePage: единый интерфейс
    с полем ввода адреса, кнопкой запуска и окном вывода.
    Конкретная команда (программа и её аргументы) задаётся
    в наследниках через чисто виртуальные методы program() и arguments().
*/
class CommandPage : public Page {
    Q_OBJECT
public:
    // Конструктор принимает название, описание и текст-подсказку поля ввода
    explicit CommandPage(const QString& title, const QString& description,
                         const QString& placeholder, QWidget* parent = nullptr);

    QString title() const override { return m_title; }

protected:
    virtual QString program() const = 0;                  // путь к исполняемому файлу
    virtual QStringList arguments(const QString& input) const = 0;  // аргументы запуска

    void appendOutput(const QString& text);  // дописать текст в окно вывода
    void setBusy(bool busy);                 // блокировать/разблокировать ввод во время работы
    QPlainTextEdit* output() const { return m_output; }

private slots:
    void onRun();       // запуск команды
    void onReadyRead(); // чтение вывода процесса
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);  // процесс завершился
    void onError(QProcess::ProcessError error);                      // ошибка запуска

private:
    QString m_title;              // название страницы (для навигации)
    QLineEdit* m_input = nullptr;        // поле ввода аргумента (адрес/домен)
    QPushButton* m_runButton = nullptr;  // кнопка запуска
    QPlainTextEdit* m_output = nullptr;  // окно вывода команды
    QProcess m_process;                  // объект внешнего процесса
};
