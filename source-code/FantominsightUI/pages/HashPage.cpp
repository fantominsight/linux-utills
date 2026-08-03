// Страница «Хеш файлов»: вычисление контрольной суммы файла
#include "HashPage.h"

#include <QByteArrayView>
#include <QComboBox>
#include <QCoreApplication>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace {

// Списки имён алгоритмов (для отображения) и соответствующих значений Qt
const char* kAlgorithmNames[] = {"MD5",   "SHA-1", "SHA-224", "SHA-256",
                                 "SHA-384", "SHA-512", "SHA3-256", "SHA3-512"};
const QCryptographicHash::Algorithm kAlgorithms[] = {
    QCryptographicHash::Md5,     QCryptographicHash::Sha1,   QCryptographicHash::Sha224,
    QCryptographicHash::Sha256,  QCryptographicHash::Sha384, QCryptographicHash::Sha512,
    QCryptographicHash::Sha3_256, QCryptographicHash::Sha3_512};

}  // namespace

// Конструктор: собираем интерфейс страницы
HashPage::HashPage(QWidget* parent)
    : Page(parent) {
    // Корневой вертикальный лейаут
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(16);

    auto* titleLabel = new QLabel(QStringLiteral("Хеш файлов"), this);
    titleLabel->setObjectName(QStringLiteral("pageTitle"));
    root->addWidget(titleLabel);

    auto* descLabel = new QLabel(
        QStringLiteral("Вычисление контрольной суммы файла (хеша)."), this);
    descLabel->setObjectName(QStringLiteral("description"));
    descLabel->setWordWrap(true);
    root->addWidget(descLabel);

    // Ряд: путь к файлу + кнопка «Обзор»
    auto* row = new QHBoxLayout;
    row->setSpacing(10);

    m_pathEdit = new QLineEdit(this);
    m_pathEdit->setObjectName(QStringLiteral("input"));
    m_pathEdit->setPlaceholderText(QStringLiteral("Выберите файл или укажите путь"));
    row->addWidget(m_pathEdit, 1);

    m_browseButton = new QPushButton(QStringLiteral("Обзор..."), this);
    m_browseButton->setObjectName(QStringLiteral("secondaryButton"));
    row->addWidget(m_browseButton);
    root->addLayout(row);

    // Ряд: выбор алгоритма + кнопка «Вычислить»
    auto* row2 = new QHBoxLayout;
    row2->setSpacing(10);

    m_algoBox = new QComboBox(this);
    for (const char* name : kAlgorithmNames) {
        m_algoBox->addItem(QString::fromLatin1(name));  // добавляем пункты из массива имён
    }
    m_algoBox->setCurrentIndex(3);  // SHA-256 по умолчанию
    row2->addWidget(m_algoBox);

    m_hashButton = new QPushButton(QStringLiteral("Вычислить"), this);
    m_hashButton->setObjectName(QStringLiteral("primaryButton"));
    row2->addWidget(m_hashButton, 1);
    root->addLayout(row2);

    // Поле для вывода результата (только для чтения)
    m_resultEdit = new QLineEdit(this);
    m_resultEdit->setObjectName(QStringLiteral("hashResult"));
    m_resultEdit->setReadOnly(true);
    m_resultEdit->setPlaceholderText(QStringLiteral("Результат появится здесь"));
    root->addWidget(m_resultEdit);

    connect(m_browseButton, &QPushButton::clicked, this, &HashPage::browse);
    connect(m_hashButton, &QPushButton::clicked, this, &HashPage::onHash);
}

// Слот «Обзор»: открывает системный диалог выбора файла и вставляет путь в поле
void HashPage::browse() {
    const QString path = QFileDialog::getOpenFileName(this, QStringLiteral("Выберите файл"));
    if (!path.isEmpty()) {
        m_pathEdit->setText(path);
    }
}

// Возвращает алгоритм, выбранный в комбобоксе (индекс → значение из массива)
QCryptographicHash::Algorithm HashPage::currentAlgorithm() const {
    return kAlgorithms[qBound(0, m_algoBox->currentIndex(), 7)];
}

/*
    Собственно вычисление хеша. Читает файл блоками по 1 МБ и «скармливает»
    их объекту QCryptographicHash. Каждые 64 МБ обрабатывает события интерфейса
    (processEvents), чтобы окно не «зависало» при хешировании больших файлов.
    Возвращает хеш в виде шестнадцатеричной строки (пустая — при ошибке открытия).
*/
QString HashPage::computeHash(const QString& path, QCryptographicHash::Algorithm algorithm) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }

    QCryptographicHash hash(algorithm);
    QByteArray buffer(1024 * 1024, Qt::Uninitialized);  // буфер чтения в 1 МБ
    qint64 total = 0;

    while (!file.atEnd()) {
        const qint64 read = file.read(buffer.data(), buffer.size());
        if (read <= 0) {
            break;  // ошибка или конец данных
        }
        hash.addData(QByteArrayView(buffer.constData(), read));  // добавляем блок в хеш
        total += read;
        if (total % (64 * 1024 * 1024) == 0) {  // раз в 64 МБ
            QCoreApplication::processEvents();  // даём интерфейсу «дышать»
        }
    }

    return QString::fromLatin1(hash.result().toHex());  // результат в hex
}

// Слот «Вычислить»: проверяем путь и запускаем расчёт хеша
void HashPage::onHash() {
    const QString path = m_pathEdit->text().trimmed();
    if (path.isEmpty()) {
        m_resultEdit->setText(QStringLiteral("Укажите файл."));
        return;
    }

    m_hashButton->setEnabled(false);  // блокируем кнопку на время расчёта
    m_resultEdit->setText(QStringLiteral("⏳ Вычисление..."));

    const QString hash = computeHash(path, currentAlgorithm());
    m_resultEdit->setText(hash.isEmpty() ? QStringLiteral("Не удалось прочитать файл.") : hash);
    m_hashButton->setEnabled(true);
}
