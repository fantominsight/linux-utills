#include "TracePage.h"

#include <QStandardPaths>

TracePage::TracePage(QWidget* parent)
    : CommandPage(QStringLiteral("Трассировка"),
                  QStringLiteral("Показывает маршрут пакетов до указанного адреса "
                                 "(traceroute или tracepath)."),
                  QStringLiteral("Введите IP или домен"), parent) {
    m_program = QStandardPaths::findExecutable(QStringLiteral("traceroute"));
    if (m_program.isEmpty()) {
        m_program = QStandardPaths::findExecutable(QStringLiteral("tracepath"));
    }
}

QString TracePage::program() const {
    return m_program;
}

QStringList TracePage::arguments(const QString& input) const {
    if (m_program.endsWith(QStringLiteral("traceroute"))) {
        return {QStringLiteral("-m"), QStringLiteral("30"), input};
    }
    return {input};
}
