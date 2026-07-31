#pragma once

#include <QCryptographicHash>

#include "Page.h"

class QComboBox;
class QLineEdit;
class QPushButton;

class HashPage final : public Page {
    Q_OBJECT
public:
    explicit HashPage(QWidget* parent = nullptr);

    QString title() const override { return QStringLiteral("Хеш файлов"); }

private slots:
    void browse();
    void onHash();

private:
    QCryptographicHash::Algorithm currentAlgorithm() const;
    QString computeHash(const QString& path, QCryptographicHash::Algorithm algorithm);

    QLineEdit* m_pathEdit = nullptr;
    QPushButton* m_browseButton = nullptr;
    QComboBox* m_algoBox = nullptr;
    QPushButton* m_hashButton = nullptr;
    QLineEdit* m_resultEdit = nullptr;
};
