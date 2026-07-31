#pragma once

#include <QMainWindow>
#include <type_traits>
#include <utility>

#include "Page.h"

class QButtonGroup;
class QStackedWidget;
class QVBoxLayout;

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

    void addGroup(const QString& title);

    template <typename T, typename... Args>
    Page* addPage(Args&&... args) {
        static_assert(std::is_base_of_v<Page, T>, "T must inherit from Page");
        return addPageImpl(new T(std::forward<Args>(args)...));
    }

private slots:
    void onNavButtonClicked(int id);

private:
    Page* addPageImpl(Page* page);
    void insertNavItem(QWidget* item);

    QStackedWidget* m_stack = nullptr;
    QButtonGroup* m_navGroup = nullptr;
    QVBoxLayout* m_navLayout = nullptr;
};
