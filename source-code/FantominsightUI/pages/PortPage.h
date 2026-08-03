#pragma once

#include <QAbstractSocket>

#include "Page.h"

// Предварительные объявления классов Qt
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QTcpSocket;
class QTimer;

/*
    Страница «Проверка портов».
    Пытается установить TCP-соединение с указанным хостом и портом.
    Порт считается открытым, если соединение установилось; при отказе
    или таймауте (4 секунды) — закрытым.
*/
class PortPage final : public Page {
    Q_OBJECT
public:
    explicit PortPage(QWidget* parent = nullptr);

    QString title() const override { return QStringLiteral("Проверка портов"); }

private slots:
    void onCheck();      // начало проверки порта
    void onConnected();  // соединение установлено — порт открыт
    void onSocketError(QAbstractSocket::SocketError error);  // ошибка соединения
    void onTimeout();    // превышено время ожидания

private:
    void finishCheck(const QString& html);  // завершение проверки с результатом
    void setBusy(bool busy);                // блокировка/разблокировка ввода

    QLineEdit* m_hostEdit = nullptr;   // поле ввода хоста
    QSpinBox* m_portSpin = nullptr;    // выбор порта
    QPushButton* m_checkButton = nullptr;  // кнопка «Проверить»
    QLabel* m_resultLabel = nullptr;   // метка с результатом (HTML)
    QTcpSocket* m_socket = nullptr;    // TCP-сокет для подключения
    QTimer* m_timer = nullptr;         // таймер ожидания ответа
};
