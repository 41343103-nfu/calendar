#pragma once
#include <QMainWindow>
#include <QDate>
#include <QVector>

#include "models.h"
#include "account.h"

class QLabel;
class QListWidget;
class QToolButton;
class DotCalendar;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent=nullptr);

private:
    void applyStyle();
    QWidget* buildTopTitle();
    QWidget* buildMonthBar();
    QWidget* buildListPanel();
    QWidget* buildBottomBar();

    void refreshDayList(const QDate& d);
    void refreshCalendarMarks();
    void saveTodosToFile();   // 存檔
    void loadTodosFromFile(); // 讀檔
    void showAllTodos();

    void checkBudgetWarning(const QDate& d);

private:
    DotCalendar *cal = nullptr;
    QLabel *monthTitle = nullptr;

    QLabel *sumLabel = nullptr;
    QListWidget *list = nullptr;

    QToolButton *btnBook = nullptr;
    QToolButton *btnPlus = nullptr;
    QToolButton *btnTodo = nullptr;

    QVector<Todo> todos;

    Account account;
    QDate currentDate;
};
