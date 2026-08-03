#pragma once

#include <QProcess>

#include "Page.h"

// Предварительные объявления классов Qt
class QLabel;
class QLineEdit;
class QPushButton;

/*
    Страница «Проверка сайта».
    Запускает системную команду ping (10 пакетов) для указанного
    IP-адреса или домена, разбирает её вывод и показывает результат
    в виде HTML: «Доступен»/«Недоступен», средний ping и статистика пакетов.
*/
class NetworkPage final : public Page {
    Q_OBJECT
public:
    explicit NetworkPage(QWidget* parent = nullptr);

    QString title() const override { return QStringLiteral("Проверка сайта"); }

private slots:
    void onCheckClicked();               // клик по кнопке «Проверить»
    void onProcessError(QProcess::ProcessError error);  // ошибка запуска ping
    void onPingFinished(int exitCode, QProcess::ExitStatus exitStatus);  // ping завершён

private:
    void setBusy(bool busy);  // блокировка/разблокировка ввода во время проверки

    QLineEdit* m_addressEdit = nullptr;  // поле ввода адреса
    QPushButton* m_checkButton = nullptr;  // кнопка «Проверить»
    QLabel* m_resultLabel = nullptr;    // метка с результатом (HTML)
    QProcess m_pingProcess;             // процесс ping
    bool m_pingFailed = false;          // флаг: произошла ошибка запуска
};
