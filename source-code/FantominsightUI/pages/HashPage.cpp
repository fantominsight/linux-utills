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

const char* kAlgorithmNames[] = {"MD5",   "SHA-1", "SHA-224", "SHA-256",
                                 "SHA-384", "SHA-512", "SHA3-256", "SHA3-512"};
const QCryptographicHash::Algorithm kAlgorithms[] = {
    QCryptographicHash::Md5,     QCryptographicHash::Sha1,   QCryptographicHash::Sha224,
    QCryptographicHash::Sha256,  QCryptographicHash::Sha384, QCryptographicHash::Sha512,
    QCryptographicHash::Sha3_256, QCryptographicHash::Sha3_512};

}  // namespace

HashPage::HashPage(QWidget* parent)
    : Page(parent) {
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

    auto* row2 = new QHBoxLayout;
    row2->setSpacing(10);

    m_algoBox = new QComboBox(this);
    for (const char* name : kAlgorithmNames) {
        m_algoBox->addItem(QString::fromLatin1(name));
    }
    m_algoBox->setCurrentIndex(3);  // SHA-256 по умолчанию
    row2->addWidget(m_algoBox);

    m_hashButton = new QPushButton(QStringLiteral("Вычислить"), this);
    m_hashButton->setObjectName(QStringLiteral("primaryButton"));
    row2->addWidget(m_hashButton, 1);
    root->addLayout(row2);

    m_resultEdit = new QLineEdit(this);
    m_resultEdit->setObjectName(QStringLiteral("hashResult"));
    m_resultEdit->setReadOnly(true);
    m_resultEdit->setPlaceholderText(QStringLiteral("Результат появится здесь"));
    root->addWidget(m_resultEdit);

    connect(m_browseButton, &QPushButton::clicked, this, &HashPage::browse);
    connect(m_hashButton, &QPushButton::clicked, this, &HashPage::onHash);
}

void HashPage::browse() {
    const QString path = QFileDialog::getOpenFileName(this, QStringLiteral("Выберите файл"));
    if (!path.isEmpty()) {
        m_pathEdit->setText(path);
    }
}

QCryptographicHash::Algorithm HashPage::currentAlgorithm() const {
    return kAlgorithms[qBound(0, m_algoBox->currentIndex(), 7)];
}

QString HashPage::computeHash(const QString& path, QCryptographicHash::Algorithm algorithm) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }

    QCryptographicHash hash(algorithm);
    QByteArray buffer(1024 * 1024, Qt::Uninitialized);
    qint64 total = 0;

    while (!file.atEnd()) {
        const qint64 read = file.read(buffer.data(), buffer.size());
        if (read <= 0) {
            break;
        }
        hash.addData(QByteArrayView(buffer.constData(), read));
        total += read;
        if (total % (64 * 1024 * 1024) == 0) {
            QCoreApplication::processEvents();
        }
    }

    return QString::fromLatin1(hash.result().toHex());
}

void HashPage::onHash() {
    const QString path = m_pathEdit->text().trimmed();
    if (path.isEmpty()) {
        m_resultEdit->setText(QStringLiteral("Укажите файл."));
        return;
    }

    m_hashButton->setEnabled(false);
    m_resultEdit->setText(QStringLiteral("⏳ Вычисление..."));

    const QString hash = computeHash(path, currentAlgorithm());
    m_resultEdit->setText(hash.isEmpty() ? QStringLiteral("Не удалось прочитать файл.") : hash);
    m_hashButton->setEnabled(true);
}
