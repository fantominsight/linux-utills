// Страница «DNS»: запрос DNS-записей через системный резолвер (QDnsLookup)
#include "DnsPage.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QHostAddress>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace {

// Возвращает короткое имя типа записи (для вывода и комбобокса)
const char* kTypeName(QDnsLookup::Type type) {
    switch (type) {
    case QDnsLookup::A:     return "A";
    case QDnsLookup::AAAA:  return "AAAA";
    case QDnsLookup::MX:    return "MX";
    case QDnsLookup::TXT:   return "TXT";
    case QDnsLookup::CNAME: return "CNAME";
    case QDnsLookup::NS:    return "NS";
    case QDnsLookup::SRV:   return "SRV";
    case QDnsLookup::PTR:   return "PTR";
    default:                return "?";
    }
}

}  // namespace

// Конструктор: собираем интерфейс (поле домена, выбор типа, кнопка, окно вывода)
DnsPage::DnsPage(QWidget* parent)
    : Page(parent) {
    // Корневой вертикальный лейаут
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(16);

    auto* titleLabel = new QLabel(QStringLiteral("DNS-запросы"), this);
    titleLabel->setObjectName(QStringLiteral("pageTitle"));
    root->addWidget(titleLabel);

    auto* descLabel = new QLabel(
        QStringLiteral("Запрос DNS-записей для домена через системный резолвер."), this);
    descLabel->setObjectName(QStringLiteral("description"));
    descLabel->setWordWrap(true);
    root->addWidget(descLabel);

    // Ряд: поле домена + комбобокс типа + кнопка
    auto* row = new QHBoxLayout;
    row->setSpacing(10);

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setObjectName(QStringLiteral("input"));
    m_nameEdit->setPlaceholderText(QStringLiteral("Введите домен"));
    row->addWidget(m_nameEdit, 1);

    m_typeBox = new QComboBox(this);  // выбор типа DNS-записи
    m_typeBox->addItems({QStringLiteral("A"), QStringLiteral("AAAA"), QStringLiteral("MX"),
                         QStringLiteral("TXT"), QStringLiteral("CNAME"), QStringLiteral("NS"),
                         QStringLiteral("SRV"), QStringLiteral("PTR")});
    row->addWidget(m_typeBox);

    m_queryButton = new QPushButton(QStringLiteral("Запросить"), this);
    m_queryButton->setObjectName(QStringLiteral("primaryButton"));
    row->addWidget(m_queryButton);
    root->addLayout(row);

    // Окно вывода результатов
    m_output = new QPlainTextEdit(this);
    m_output->setObjectName(QStringLiteral("output"));
    m_output->setReadOnly(true);
    m_output->setMaximumBlockCount(2000);
    root->addWidget(m_output, 1);

    // Запуск по кнопке и по нажатию Enter в поле ввода
    connect(m_queryButton, &QPushButton::clicked, this, &DnsPage::onQuery);
    connect(m_nameEdit, &QLineEdit::returnPressed, this, &DnsPage::onQuery);
}

// Возвращает выбранный в комбобоксе тип записи (индекс комбобокса → значение QDnsLookup::Type)
QDnsLookup::Type DnsPage::currentType() const {
    const int index = m_typeBox->currentIndex();
    // Массив типов в том же порядке, что и пункты комбобокса
    static const QDnsLookup::Type kTypes[] = {
        QDnsLookup::A,  QDnsLookup::AAAA, QDnsLookup::MX, QDnsLookup::TXT,
        QDnsLookup::CNAME, QDnsLookup::NS, QDnsLookup::SRV, QDnsLookup::PTR};
    return kTypes[qBound(0, index, 7)];  // защищаем индекс от выхода за границы
}

// Слот «Запросить»: создаём QDnsLookup и запускаем асинхронный запрос
void DnsPage::onQuery() {
    const QString name = m_nameEdit->text().trimmed();
    if (name.isEmpty()) {
        m_output->setPlainText(QStringLiteral("⚠ Укажите домен."));
        return;
    }
    if (m_lookup) {  // если остался старый запрос — удаляем его (после обработки событий)
        m_lookup->deleteLater();
    }

    // Асинхронный DNS-запрос выбранного типа; результат придёт сигналом finished
    m_lookup = new QDnsLookup(currentType(), name, this);
    connect(m_lookup, &QDnsLookup::finished, this, &DnsPage::onFinished);

    m_queryButton->setEnabled(false);  // блокируем кнопку до ответа
    m_output->clear();
    m_output->appendPlainText(QStringLiteral("Запрос %1 %2 ...")
                                  .arg(QLatin1String(kTypeName(currentType())), name));
    m_lookup->lookup();  // запускаем запрос
}

// Слот: запрос завершился (успешно или с ошибкой)
void DnsPage::onFinished() {
    m_queryButton->setEnabled(true);  // снова разрешаем кнопку
    if (!m_lookup) {
        return;
    }
    m_lookup->deleteLater();  // запрос отработал — удаляем объект

    if (m_lookup->error() != QDnsLookup::NoError) {  // ошибка резолвера
        m_output->appendPlainText(QStringLiteral("⚠ %1").arg(m_lookup->errorString()));
        return;
    }
    printRecords();  // печатаем найденные записи
    m_output->appendPlainText(QStringLiteral("\n— Готово —"));
}

// Выводит записи в зависимости от типа запроса
void DnsPage::printRecords() {
    switch (m_lookup->type()) {
    case QDnsLookup::A:       // IPv4-адреса
    case QDnsLookup::AAAA:    // IPv6-адреса
        for (const auto& record : m_lookup->hostAddressRecords()) {
            append(QStringLiteral("%1  %2")
                       .arg(QLatin1String(kTypeName(m_lookup->type())),
                            record.value().toString()));
        }
        break;
    case QDnsLookup::MX:  // почтовые серверы
        for (const auto& record : m_lookup->mailExchangeRecords()) {
            append(QStringLiteral("MX  %1  приоритет %2")
                       .arg(record.exchange(), QString::number(record.preference())));
        }
        break;
    case QDnsLookup::TXT:  // текстовые записи
        for (const auto& record : m_lookup->textRecords()) {
            for (const auto& value : record.values()) {
                append(QStringLiteral("TXT  %1").arg(value));
            }
        }
        break;
    case QDnsLookup::CNAME:  // канонические имена (алиасы)
        for (const auto& record : m_lookup->canonicalNameRecords()) {
            append(QStringLiteral("CNAME  %1").arg(record.value()));
        }
        break;
    case QDnsLookup::NS:  // серверы имён
        for (const auto& record : m_lookup->nameServerRecords()) {
            append(QStringLiteral("NS  %1").arg(record.name()));
        }
        break;
    case QDnsLookup::SRV:  // сервисные записи
        for (const auto& record : m_lookup->serviceRecords()) {
            append(QStringLiteral("SRV  %1:%2  приоритет %3, вес %4")
                       .arg(record.target(), QString::number(record.port()),
                            QString::number(record.priority()), QString::number(record.weight())));
        }
        break;
    case QDnsLookup::PTR:  // записи обратного разрешения
        for (const auto& record : m_lookup->pointerRecords()) {
            append(QStringLiteral("PTR  %1").arg(record.name()));
        }
        break;
    default:
        append(QStringLiteral("Записей нет."));
    }
}

// Дописывает строку в окно вывода
void DnsPage::append(const QString& text) {
    m_output->appendPlainText(text);
}
