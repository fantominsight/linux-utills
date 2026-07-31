#pragma once

#include <QAbstractSocket>

#include "Page.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QTcpSocket;
class QTimer;

class PortPage final : public Page {
    Q_OBJECT
public:
    explicit PortPage(QWidget* parent = nullptr);

    QString title() const override { return QStringLiteral("Проверка портов"); }

private slots:
    void onCheck();
    void onConnected();
    void onSocketError(QAbstractSocket::SocketError error);
    void onTimeout();

private:
    void finishCheck(const QString& html);
    void setBusy(bool busy);

    QLineEdit* m_hostEdit = nullptr;
    QSpinBox* m_portSpin = nullptr;
    QPushButton* m_checkButton = nullptr;
    QLabel* m_resultLabel = nullptr;
    QTcpSocket* m_socket = nullptr;
    QTimer* m_timer = nullptr;
};
