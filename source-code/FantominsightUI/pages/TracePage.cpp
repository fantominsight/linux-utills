// Страница «Трассировка»: маршрут пакетов до указанного адреса
#include "TracePage.h"

#include <QStandardPaths>

// Конструктор: передаём настройки в CommandPage и ищем утилиту трассировки
TracePage::TracePage(QWidget* parent)
    : CommandPage(QStringLiteral("Трассировка"),
                  QStringLiteral("Показывает маршрут пакетов до указанного адреса "
                                 "(traceroute или tracepath)."),
                  QStringLiteral("Введите IP или домен"), parent) {
    // Сначала ищем traceroute; если не нашли — берём tracepath (есть почти везде)
    m_program = QStandardPaths::findExecutable(QStringLiteral("traceroute"));
    if (m_program.isEmpty()) {
        m_program = QStandardPaths::findExecutable(QStringLiteral("tracepath"));
    }
}

// Возвращает путь к найденной утилите
QString TracePage::program() const {
    return m_program;
}

// Аргументы зависят от утилиты:
// traceroute -m 30 <адрес> (до 30 хопов), tracepath <адрес> (без опций)
QStringList TracePage::arguments(const QString& input) const {
    if (m_program.endsWith(QStringLiteral("traceroute"))) {
        return {QStringLiteral("-m"), QStringLiteral("30"), input};
    }
    return {input};
}
