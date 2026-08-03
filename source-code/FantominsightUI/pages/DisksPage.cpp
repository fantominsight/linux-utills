// Страница «Диски»: список смонтированных дисковых устройств
#include "DisksPage.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QStorageInfo>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {

// Список «псевдо»-файловых систем (виртуальные ФС ядра), которые не являются дисками
bool isPseudoFs(const QString& type) {
    static const QStringList kPseudo = {
        QStringLiteral("sysfs"),   QStringLiteral("proc"),   QStringLiteral("devpts"),
        QStringLiteral("devtmpfs"), QStringLiteral("cgroup"), QStringLiteral("cgroup2"),
        QStringLiteral("securityfs"), QStringLiteral("debugfs"), QStringLiteral("pstore"),
        QStringLiteral("bpf"),     QStringLiteral("fusectl"), QStringLiteral("configfs"),
        QStringLiteral("hugetlbfs"), QStringLiteral("mqueue"), QStringLiteral("autofs")};
    return kPseudo.contains(type);
}

// Форматирует размер в байтах в читаемый вид: ГБ или МБ
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

// Конструктор: собираем интерфейс с таблицей и кнопкой обновления
DisksPage::DisksPage(QWidget* parent)
    : Page(parent) {
    // Корневой вертикальный лейаут
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(16);

    auto* titleLabel = new QLabel(QStringLiteral("Диски"), this);
    titleLabel->setObjectName(QStringLiteral("pageTitle"));
    root->addWidget(titleLabel);

    // Ряд: описание слева, кнопка «Обновить» справа
    auto* headerRow = new QHBoxLayout;
    auto* descLabel = new QLabel(QStringLiteral("Смонтированные дисковые устройства."), this);
    descLabel->setObjectName(QStringLiteral("description"));
    headerRow->addWidget(descLabel, 1);

    m_refreshButton = new QPushButton(QStringLiteral("Обновить"), this);
    m_refreshButton->setObjectName(QStringLiteral("secondaryButton"));
    headerRow->addWidget(m_refreshButton);
    root->addLayout(headerRow);

    // Таблица с 7 колонками: Диск | Точка монтирования | ФС | Всего | Занято | Свободно | Занято%
    m_table = new QTableWidget(this);
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels(
        {QStringLiteral("Диск"), QStringLiteral("Точка монтирования"),
         QStringLiteral("Файловая система"), QStringLiteral("Всего"),
         QStringLiteral("Занято"), QStringLiteral("Свободно"), QStringLiteral("Занято")});
    m_table->horizontalHeader()->setStretchLastSection(true);  // последняя колонка растягивается
    m_table->verticalHeader()->setVisible(false);              // скрываем номера строк
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);  // таблица только для чтения
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows); // выделение целиком строк
    m_table->setAlternatingRowColors(true);                       // «зебра» для читаемости
    root->addWidget(m_table, 1);

    connect(m_refreshButton, &QPushButton::clicked, this, &DisksPage::refresh);
    refresh();  // заполняем таблицу сразу при открытии страницы
}

// Перечитывает список смонтированных томов и заполняет таблицу
void DisksPage::refresh() {
    const auto volumes = QStorageInfo::mountedVolumes();  // все смонтированные тома

    // Оставляем только реальные диски: убираем невалидные/неготовые тома и псевдо-ФС
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

    m_table->setRowCount(filtered.size());  // задаём число строк таблицы
    for (int row = 0; row < filtered.size(); ++row) {
        const auto& volume = filtered.at(row);
        // Занято = всего − доступно (с защитой от отрицательных значений)
        const qint64 used = volume.bytesTotal() > volume.bytesAvailable()
                                ? volume.bytesTotal() - volume.bytesAvailable()
                                : 0;
        // Процент занятости с ограничением диапазона [0, 100]
        const int percent = volume.bytesTotal() > 0
                                ? qBound(0, int((100.0 * used) / volume.bytesTotal()), 100)
                                : 0;

        // Готовим значения всех колонок
        const QStringList values = {
            volume.device(),         // устройство (например /dev/sda1)
            volume.rootPath(),       // точка монтирования (например /)
            volume.fileSystemType(), // тип ФС (ext4, btrfs...)
            formatBytes(volume.bytesTotal()),      // всего
            formatBytes(used),                     // занято
            formatBytes(volume.bytesAvailable()),  // свободно
            QStringLiteral("%1%").arg(percent)};   // процент занятости

        // Создаём ячейки и кладём их в строку
        for (int col = 0; col < values.size(); ++col) {
            auto* item = new QTableWidgetItem(values.at(col));
            if (col == 6) {  // колонка с процентами — по центру
                item->setTextAlignment(Qt::AlignCenter);
            }
            m_table->setItem(row, col, item);
        }
    }
}
