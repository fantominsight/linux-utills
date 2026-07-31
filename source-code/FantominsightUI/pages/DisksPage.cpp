#include "DisksPage.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QStorageInfo>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {

bool isPseudoFs(const QString& type) {
    static const QStringList kPseudo = {
        QStringLiteral("sysfs"),   QStringLiteral("proc"),   QStringLiteral("devpts"),
        QStringLiteral("devtmpfs"), QStringLiteral("cgroup"), QStringLiteral("cgroup2"),
        QStringLiteral("securityfs"), QStringLiteral("debugfs"), QStringLiteral("pstore"),
        QStringLiteral("bpf"),     QStringLiteral("fusectl"), QStringLiteral("configfs"),
        QStringLiteral("hugetlbfs"), QStringLiteral("mqueue"), QStringLiteral("autofs")};
    return kPseudo.contains(type);
}

QString formatBytes(qint64 bytes) {
    if (bytes <= 0) {
        return QStringLiteral("—");
    }
    const double gb = double(bytes) / (1024.0 * 1024.0 * 1024.0);
    if (gb >= 1.0) {
        return QString::number(gb, 'f', 2) + QStringLiteral(" ГБ");
    }
    const double mb = double(bytes) / (1024.0 * 1024.0);
    return QString::number(mb, 'f', 1) + QStringLiteral(" МБ");
}

}  // namespace

DisksPage::DisksPage(QWidget* parent)
    : Page(parent) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(16);

    auto* titleLabel = new QLabel(QStringLiteral("Диски"), this);
    titleLabel->setObjectName(QStringLiteral("pageTitle"));
    root->addWidget(titleLabel);

    auto* headerRow = new QHBoxLayout;
    auto* descLabel = new QLabel(QStringLiteral("Смонтированные дисковые устройства."), this);
    descLabel->setObjectName(QStringLiteral("description"));
    headerRow->addWidget(descLabel, 1);

    m_refreshButton = new QPushButton(QStringLiteral("Обновить"), this);
    m_refreshButton->setObjectName(QStringLiteral("secondaryButton"));
    headerRow->addWidget(m_refreshButton);
    root->addLayout(headerRow);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels(
        {QStringLiteral("Диск"), QStringLiteral("Точка монтирования"),
         QStringLiteral("Файловая система"), QStringLiteral("Всего"),
         QStringLiteral("Занято"), QStringLiteral("Свободно"), QStringLiteral("Занято")});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->setVisible(false);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setAlternatingRowColors(true);
    root->addWidget(m_table, 1);

    connect(m_refreshButton, &QPushButton::clicked, this, &DisksPage::refresh);
    refresh();
}

void DisksPage::refresh() {
    const auto volumes = QStorageInfo::mountedVolumes();

    QList<QStorageInfo> filtered;
    filtered.reserve(volumes.size());
    for (const auto& volume : volumes) {
        if (!volume.isValid() || !volume.isReady()) {
            continue;
        }
        if (isPseudoFs(volume.fileSystemType())) {
            continue;
        }
        filtered.append(volume);
    }

    m_table->setRowCount(filtered.size());
    for (int row = 0; row < filtered.size(); ++row) {
        const auto& volume = filtered.at(row);
        const qint64 used = volume.bytesTotal() > volume.bytesAvailable()
                                ? volume.bytesTotal() - volume.bytesAvailable()
                                : 0;
        const int percent = volume.bytesTotal() > 0
                                ? qBound(0, int((100.0 * used) / volume.bytesTotal()), 100)
                                : 0;

        const QStringList values = {
            volume.device(),        volume.rootPath(),
            volume.fileSystemType(), formatBytes(volume.bytesTotal()),
            formatBytes(used),       formatBytes(volume.bytesAvailable()),
            QStringLiteral("%1%").arg(percent)};

        for (int col = 0; col < values.size(); ++col) {
            auto* item = new QTableWidgetItem(values.at(col));
            if (col == 6) {
                item->setTextAlignment(Qt::AlignCenter);
            }
            m_table->setItem(row, col, item);
        }
    }
}
