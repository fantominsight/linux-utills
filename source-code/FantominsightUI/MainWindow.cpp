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

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setWindowTitle(QStringLiteral("Набор сетевых и системных инструментов"));
    resize(820, 580);
    setStyleSheet(QString::fromUtf8(kStyleSheet));

    auto* central = new QWidget(this);
    auto* rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    auto* navPanel = new QWidget(central);
    navPanel->setObjectName(QStringLiteral("navPanel"));
    navPanel->setFixedWidth(212);

    auto* navScroll = new QScrollArea(navPanel);
    navScroll->setWidgetResizable(true);
    navScroll->setFrameShape(QFrame::NoFrame);
    navScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto* navContent = new QWidget(navScroll);
    navContent->setObjectName(QStringLiteral("navPanel"));
    m_navLayout = new QVBoxLayout(navContent);
    m_navLayout->setContentsMargins(8, 10, 8, 14);
    m_navLayout->setSpacing(2);
    m_navLayout->addStretch(1);
    navScroll->setWidget(navContent);

    auto* navVBox = new QVBoxLayout(navPanel);
    navVBox->setContentsMargins(0, 0, 0, 0);
    navVBox->setSpacing(0);
    navVBox->addWidget(navScroll);

    rootLayout->addWidget(navPanel);

    m_stack = new QStackedWidget(central);
    rootLayout->addWidget(m_stack, 1);

    setCentralWidget(central);

    m_navGroup = new QButtonGroup(this);
    m_navGroup->setExclusive(true);
    connect(m_navGroup, &QButtonGroup::idClicked, this, &MainWindow::onNavButtonClicked);
}

void MainWindow::addGroup(const QString& title) {
    auto* label = new QLabel(title);
    label->setObjectName(QStringLiteral("navGroup"));
    insertNavItem(label);
}

void MainWindow::insertNavItem(QWidget* item) {
    m_navLayout->insertWidget(m_navLayout->count() - 1, item);
}

Page* MainWindow::addPageImpl(Page* page) {
    const int index = m_stack->count();
    m_stack->addWidget(page);

    auto* button = new QPushButton(page->title());
    button->setCheckable(true);
    button->setObjectName(QStringLiteral("navButton"));
    m_navGroup->addButton(button, index);
    insertNavItem(button);

    if (index == 0) {
        button->setChecked(true);
        m_stack->setCurrentIndex(0);
    }

    return page;
}

void MainWindow::onNavButtonClicked(int id) {
    if (id >= 0 && id < m_stack->count()) {
        m_stack->setCurrentIndex(id);
    }
}
