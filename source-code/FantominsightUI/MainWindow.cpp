#include "MainWindow.h"

#include <QButtonGroup>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QVBoxLayout>

namespace {

// Глобальная таблица стилей приложения (CSS-подобный синтаксис Qt Style Sheets).
// Задаёт тёмную панель навигации, цвета кнопок, полей ввода и прочих виджетов.
const char kStyleSheet[] = R"(
    QMainWindow { background-color: #f5f6f8; }

    #navPanel { background-color: #2b2f3a; }
    #navPanel QScrollArea { border: none; background: transparent; }
    #navPanel QScrollArea > QWidget > QWidget { background: transparent; }

    #navGroup {
        color: #8b93a5;
        font-size: 9.5pt;
        font-weight: 600;
        padding: 12px 12px 4px 12px;
    }

    #navButton {
        color: #dfe3ea;
        background-color: transparent;
        border: none;
        border-radius: 6px;
        padding: 8px 14px 8px 20px;
        font-size: 11pt;
        text-align: left;
    }
    #navButton:hover { background-color: #3a4150; }
    #navButton:checked { background-color: #4b8cf8; color: #ffffff; }

    #navPanel QScrollBar:vertical { background: transparent; width: 8px; margin: 2px; }
    #navPanel QScrollBar::handle:vertical { background: #3a4150; border-radius: 4px; min-height: 30px; }
    #navPanel QScrollBar::add-line:vertical,
    #navPanel QScrollBar::sub-line:vertical { height: 0; }

    #pageTitle { font-size: 19pt; font-weight: 600; color: #1f232b; }
    #description { font-size: 11pt; color: #6a717d; }

    #input, #addressEdit {
        font-size: 12pt;
        padding: 8px 12px;
        border: 1px solid #c9cfd8;
        border-radius: 6px;
        background-color: #ffffff;
        color: #1f232b;
    }
    #input:focus, #addressEdit:focus { border-color: #4b8cf8; }

    #output, #infoText, #hashResult {
        font-family: 'Ubuntu Mono', 'DejaVu Sans Mono', 'Courier New', monospace;
        font-size: 10.5pt;
        color: #1f232b;
        background-color: #ffffff;
        border: 1px solid #c9cfd8;
        border-radius: 6px;
        padding: 6px;
    }

    #primaryButton {
        font-size: 12pt;
        font-weight: 600;
        color: #ffffff;
        background-color: #4b8cf8;
        border: none;
        border-radius: 6px;
        padding: 9px 26px;
    }
    #primaryButton:hover { background-color: #3d7ae0; }
    #primaryButton:disabled { background-color: #9db9e8; }

    #secondaryButton {
        font-size: 11pt;
        color: #1f232b;
        background-color: #ffffff;
        border: 1px solid #c9cfd8;
        border-radius: 6px;
        padding: 8px 18px;
    }
    #secondaryButton:hover { background-color: #eef1f5; }

    #resultLabel { font-size: 11pt; color: #3c4043; }

    QSpinBox, QComboBox, QCheckBox {
        font-size: 11pt;
        color: #1f232b;
        background-color: #ffffff;
        border: 1px solid #c9cfd8;
        border-radius: 6px;
        padding: 4px 8px;
    }
    QCheckBox { border: none; background: transparent; padding: 2px; }
)";

}  // namespace

/*
    Конструктор главного окна.
    Собирает каркас: центральный виджет с горизонтальным лейаутом,
    внутри слева — панель навигации, справа — стек страниц.
*/
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setWindowTitle(QStringLiteral("Набор сетевых и системных инструментов"));
    resize(820, 580);
    setStyleSheet(QString::fromUtf8(kStyleSheet));  // применяем глобальный стиль

    // Центральный виджет и корневой горизонтальный лейаут (навигация | контент)
    auto* central = new QWidget(this);
    auto* rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // Левая панель навигации фиксированной ширины
    auto* navPanel = new QWidget(central);
    navPanel->setObjectName(QStringLiteral("navPanel"));
    navPanel->setFixedWidth(212);

    // Скроллируемая область внутри панели — на случай множества пунктов
    auto* navScroll = new QScrollArea(navPanel);
    navScroll->setWidgetResizable(true);
    navScroll->setFrameShape(QFrame::NoFrame);
    navScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Контейнер с вертикальным списком: заголовки групп и кнопки страниц
    auto* navContent = new QWidget(navScroll);
    navContent->setObjectName(QStringLiteral("navPanel"));
    m_navLayout = new QVBoxLayout(navContent);
    m_navLayout->setContentsMargins(8, 10, 8, 14);
    m_navLayout->setSpacing(2);
    m_navLayout->addStretch(1);  // прижимаем список к верху, снизу оставляем пустоту
    navScroll->setWidget(navContent);

    // Оборачиваем скролл-область в вертикальный лейаут панели
    auto* navVBox = new QVBoxLayout(navPanel);
    navVBox->setContentsMargins(0, 0, 0, 0);
    navVBox->setSpacing(0);
    navVBox->addWidget(navScroll);

    rootLayout->addWidget(navPanel);  // панель — слева

    // Стек страниц — справа, растягивается на всё оставшееся место
    m_stack = new QStackedWidget(central);
    rootLayout->addWidget(m_stack, 1);

    setCentralWidget(central);  // устанавливаем центральный виджет окна

    // Группа кнопок навигации с эксклюзивным поведением (как радиокнопки:
    // всегда нажата ровно одна). Клик по кнопке переключает страницу.
    m_navGroup = new QButtonGroup(this);
    m_navGroup->setExclusive(true);
    connect(m_navGroup, &QButtonGroup::idClicked, this, &MainWindow::onNavButtonClicked);
}

// Добавляет заголовок группы (например "🌐 Сеть") в панель навигации
void MainWindow::addGroup(const QString& title) {
    auto* label = new QLabel(title);
    label->setObjectName(QStringLiteral("navGroup"));
    insertNavItem(label);
}

// Вставляет элемент навигации перед нижней "пружиной" (растяжкой),
// чтобы новые пункты добавлялись в конец списка
void MainWindow::insertNavItem(QWidget* item) {
    m_navLayout->insertWidget(m_navLayout->count() - 1, item);
}

/*
    Реальная реализация добавления страницы:
    кладёт страницу в стек, создаёт для неё кнопку в навигации
    и связывает кнопку с индексом страницы.
*/
Page* MainWindow::addPageImpl(Page* page) {
    const int index = m_stack->count();  // индекс новой страницы в стеке
    m_stack->addWidget(page);            // добавляем страницу в стек

    // Кнопка навигации с названием страницы
    auto* button = new QPushButton(page->title());
    button->setCheckable(true);  // кнопка "залипающая" — отмечает выбранную страницу
    button->setObjectName(QStringLiteral("navButton"));
    m_navGroup->addButton(button, index);  // id кнопки = индекс страницы
    insertNavItem(button);

    // Первая добавленная страница становится активной по умолчанию
    if (index == 0) {
        button->setChecked(true);
        m_stack->setCurrentIndex(0);
    }

    return page;
}

// Слот: при клике по кнопке навигации показываем соответствующую страницу
void MainWindow::onNavButtonClicked(int id) {
    if (id >= 0 && id < m_stack->count()) {
        m_stack->setCurrentIndex(id);
    }
}
