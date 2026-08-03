// Страница «Генератор паролей»: генерация случайного пароля
#include "PasswordPage.h"

#include <QCheckBox>
#include <QClipboard>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSpinBox>
#include <QVBoxLayout>
#include <utility>

namespace {

// Перемешивает символы строки (алгоритм Фишера–Йетса), чтобы символы
// из разных наборов были разбросаны по паролю, а не сгруппированы
QString shuffle(QString input) {
    auto* rng = QRandomGenerator::global();  // глобальный генератор случайных чисел
    for (int i = input.size() - 1; i > 0; --i) {
        const int j = int(rng->bounded(quint32(i + 1)));  // случайный индекс от 0 до i
        std::swap(input[i], input[j]);                    // меняем местами
    }
    return input;
}

}  // namespace

// Конструктор: собираем интерфейс страницы
PasswordPage::PasswordPage(QWidget* parent)
    : Page(parent) {
    // Корневой вертикальный лейаут
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(16);

    auto* titleLabel = new QLabel(QStringLiteral("Генератор паролей"), this);
    titleLabel->setObjectName(QStringLiteral("pageTitle"));
    root->addWidget(titleLabel);

    auto* descLabel = new QLabel(
        QStringLiteral("Генерация случайного пароля по выбранным наборам символов."), this);
    descLabel->setObjectName(QStringLiteral("description"));
    descLabel->setWordWrap(true);
    root->addWidget(descLabel);

    // Ряд: надпись «Длина:» + спинбокс с диапазоном 8..64
    auto* lenRow = new QHBoxLayout;
    lenRow->setSpacing(10);
    lenRow->addWidget(new QLabel(QStringLiteral("Длина:"), this));
    m_lengthSpin = new QSpinBox(this);
    m_lengthSpin->setRange(8, 64);
    m_lengthSpin->setValue(16);  // длина по умолчанию
    m_lengthSpin->setFixedWidth(90);
    lenRow->addWidget(m_lengthSpin);
    lenRow->addStretch(1);  // прижимаем к левому краю
    root->addLayout(lenRow);

    // Ряд из четырёх чекбоксов выбора наборов символов
    auto* boxes = new QHBoxLayout;
    boxes->setSpacing(14);
    m_lowerBox = new QCheckBox(QStringLiteral("a-z"), this);
    m_upperBox = new QCheckBox(QStringLiteral("A-Z"), this);
    m_digitBox = new QCheckBox(QStringLiteral("0-9"), this);
    m_symbolBox = new QCheckBox(QStringLiteral("Символы"), this);
    for (auto* box : {m_lowerBox, m_upperBox, m_digitBox, m_symbolBox}) {
        box->setChecked(true);  // все наборы включены по умолчанию
        boxes->addWidget(box);
    }
    boxes->addStretch(1);
    root->addLayout(boxes);

    // Ряд: поле результата + кнопки «Копировать» и «Сгенерировать»
    auto* row = new QHBoxLayout;
    row->setSpacing(10);

    m_resultEdit = new QLineEdit(this);
    m_resultEdit->setObjectName(QStringLiteral("hashResult"));
    m_resultEdit->setReadOnly(true);  // результат нельзя редактировать вручную
    m_resultEdit->setPlaceholderText(QStringLiteral("Сгенерированный пароль"));
    row->addWidget(m_resultEdit, 1);

    m_copyButton = new QPushButton(QStringLiteral("Копировать"), this);
    m_copyButton->setObjectName(QStringLiteral("secondaryButton"));
    row->addWidget(m_copyButton);

    m_generateButton = new QPushButton(QStringLiteral("Сгенерировать"), this);
    m_generateButton->setObjectName(QStringLiteral("primaryButton"));
    row->addWidget(m_generateButton);
    root->addLayout(row);

    root->addStretch(1);  // оставшееся место — пустое (контент сверху)

    connect(m_generateButton, &QPushButton::clicked, this, &PasswordPage::onGenerate);
    connect(m_copyButton, &QPushButton::clicked, this, &PasswordPage::onCopy);
}

// Слот «Сгенерировать»: собираем алфавит и строим случайный пароль
void PasswordPage::onGenerate() {
    // Доступные наборы символов
    static const QString kLower = QStringLiteral("abcdefghijklmnopqrstuvwxyz");
    static const QString kUpper = QStringLiteral("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    static const QString kDigits = QStringLiteral("0123456789");
    static const QString kSymbols = QStringLiteral("!@#$%^&*()-_=+[]{};:,.<>?");

    QString all;     // объединённый алфавит из включённых наборов
    QString firstOf; // по одному символу из каждого включённого набора

    // Лямбда: если набор включён — добавляем его к алфавиту и запоминаем первый символ
    const auto addSet = [&](bool enabled, const QString& set) {
        if (enabled) {
            all += set;
            firstOf += set.front();
        }
    };
    addSet(m_lowerBox->isChecked(), kLower);
    addSet(m_upperBox->isChecked(), kUpper);
    addSet(m_digitBox->isChecked(), kDigits);
    addSet(m_symbolBox->isChecked(), kSymbols);

    if (all.isEmpty()) {  // ни один набор не выбран
        m_resultEdit->setText(QStringLiteral("Выберите хотя бы один набор символов."));
        return;
    }

    const int length = m_lengthSpin->value();
    auto* rng = QRandomGenerator::global();

    QString password;
    password.reserve(length);
    password += firstOf;  // гарантируем по одному символу из каждого набора
    while (password.size() < length) {  // добираем остальные символы случайно
        password += all[int(rng->bounded(quint32(all.size())))];
    }

    m_resultEdit->setText(shuffle(password));  // перемешиваем и показываем
}

// Слот «Копировать»: кладём сгенерированный пароль в буфер обмена
void PasswordPage::onCopy() {
    QGuiApplication::clipboard()->setText(m_resultEdit->text());
}
