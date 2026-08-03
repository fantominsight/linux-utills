// Страница «Информация о ПК»: сбор и отображение базовой информации о системе
#include "SysInfoPage.h"

#include <QFile>
#include <QLabel>
#include <QSysInfo>
#include <QTextCursor>
#include <QTextEdit>
#include <QVBoxLayout>

namespace {

// Читает файл (например /proc/cpuinfo) и возвращает значение ключа
// вида "ключ : значение" из первой совпавшей строки
QString readFirstMatch(const QString& path, const QString& key) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }
    const QByteArray prefix = key.toLatin1();
    while (true) {
        const QByteArray line = file.readLine();
        if (line.isEmpty()) {
            break;  // конец файла — ключ не найден
        }
        if (line.startsWith(prefix)) {
            const int pos = line.indexOf(':');  // отделяем значение после двоеточия
            return QString::fromUtf8(line.mid(pos + 1).trimmed());
        }
    }
    return {};
}

// Модель процессора: из поля "model name" (x86) или "Hardware" (ARM и др.)
QString cpuModel() {
    QString model = readFirstMatch(QStringLiteral("/proc/cpuinfo"), QStringLiteral("model name"));
    if (model.isEmpty()) {
        model = readFirstMatch(QStringLiteral("/proc/cpuinfo"), QStringLiteral("Hardware"));
    }
    return model;
}

// Число ядер CPU: считает строки "processor" в /proc/cpuinfo
int cpuCoreCount() {
    QFile file(QStringLiteral("/proc/cpuinfo"));
    if (!file.open(QIODevice::ReadOnly)) {
        return 0;
    }
    int cores = 0;
    while (true) {
        const QByteArray line = file.readLine();
        if (line.isEmpty()) {
            break;
        }
        if (line.startsWith("processor")) {  // каждая такая строка — одно ядро/поток
            ++cores;
        }
    }
    return cores;
}

// Форматирует время работы (uptime) в секундах как «N д H ч M мин»
QString formatUptime(qint64 seconds) {
    const qint64 days = seconds / 86400;
    const qint64 hours = (seconds % 86400) / 3600;
    const qint64 minutes = (seconds % 3600) / 60;
    return QStringLiteral("%1 д %2 ч %3 мин").arg(days).arg(hours).arg(minutes);
}

}  // namespace

// Конструктор: создаём текстовое поле и сразу заполняем его информацией
SysInfoPage::SysInfoPage(QWidget* parent)
    : Page(parent) {
    // Корневой вертикальный лейаут
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(16);

    auto* titleLabel = new QLabel(QStringLiteral("Информация о ПК"), this);
    titleLabel->setObjectName(QStringLiteral("pageTitle"));
    root->addWidget(titleLabel);

    // Многострочное поле для вывода (только для чтения)
    m_text = new QTextEdit(this);
    m_text->setObjectName(QStringLiteral("infoText"));
    m_text->setReadOnly(true);
    root->addWidget(m_text, 1);

    buildInfo();  // заполняем информацию
}

// Собирает все данные и выводит их в виде строк «Имя: значение»
void SysInfoPage::buildInfo() {
    // Данные о системе от Qt
    addField(QStringLiteral("Хост"), QSysInfo::machineHostName());
    addField(QStringLiteral("Продукт"), QSysInfo::prettyProductName());
    addField(QStringLiteral("Ядро"), QStringLiteral("%1 %2")
                                          .arg(QSysInfo::kernelType(), QSysInfo::kernelVersion()));
    addField(QStringLiteral("Архитектура"), QSysInfo::currentCpuArchitecture());

    // Модель процессора (если удалось узнать) и число ядер
    const QString model = cpuModel();
    if (!model.isEmpty()) {
        addField(QStringLiteral("CPU"), model);
    }
    addField(QStringLiteral("Ядер CPU"), QString::number(cpuCoreCount()));

    // Время работы: первое число в /proc/uptime — секунды с момента загрузки
    QFile uptime(QStringLiteral("/proc/uptime"));
    if (uptime.open(QIODevice::ReadOnly)) {
        const QStringList parts =
            QString::fromLatin1(uptime.readLine()).trimmed().split(QLatin1Char(' '));
        if (!parts.isEmpty()) {
            addField(QStringLiteral("Время работы"), formatUptime(qint64(parts.first().toDouble())));
        }
    }
    m_text->moveCursor(QTextCursor::Start);  // курсор в начало, чтобы видеть первые строки
}

// Добавляет строку «<b>Имя</b>: значение» в текстовое поле (значения экранируются от HTML)
void SysInfoPage::addField(const QString& name, const QString& value) {
    m_text->append(QStringLiteral("<b>%1:</b> %2").arg(name.toHtmlEscaped(), value.toHtmlEscaped()));
}
