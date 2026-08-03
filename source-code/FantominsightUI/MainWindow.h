#pragma once

#include <QMainWindow>
#include <type_traits>
#include <utility>

#include "Page.h"

// Предварительные объявления классов Qt (чтобы не тянуть тяжёлые заголовки)
class QButtonGroup;
class QStackedWidget;
class QVBoxLayout;

/*
    Главное окно приложения.
    Слева — тёмная панель навигации (названия групп и кнопки страниц),
    справа — QStackedWidget, в котором одновременно показана только
    одна выбранная страница-инструмент.

    Страницы добавляются группами через addGroup() и addPage<T>(),
    при этом каждая страница автоматически получает кнопку в навигации.
*/
class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

    // Добавляет заголовок группы в боковую панель (например "🌐 Сеть")
    void addGroup(const QString& title);

    /*
        Добавляет новую страницу типа T (должен наследоваться от Page).
        Благодаря шаблону можно передавать любые дополнительные аргументы
        в конструктор страницы, например addPage<PingPage>().
    */
    template <typename T, typename... Args>
    Page* addPage(Args&&... args) {
        // Проверка на этапе компиляции: T обязана быть наследником Page
        static_assert(std::is_base_of_v<Page, T>, "T must inherit from Page");
        // Создаём страницу через new и передаём её в реальную реализацию
        return addPageImpl(new T(std::forward<Args>(args)...));
    }

private slots:
    void onNavButtonClicked(int id);  // слот: переключение страницы по клику в навигации

private:
    Page* addPageImpl(Page* page);  // фактическое добавление страницы и её кнопки
    void insertNavItem(QWidget* item);  // вставка элемента в конец списка навигации

    QStackedWidget* m_stack = nullptr;  // стек страниц — показывает одну активную страницу
    QButtonGroup* m_navGroup = nullptr; // группа кнопок навигации (радио-поведение: выбрана одна)
    QVBoxLayout* m_navLayout = nullptr; // вертикальный список заголовков и кнопок в панели
};
