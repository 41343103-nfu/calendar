#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>
#include <QVector>


class QLabel;
class QListWidget;
class QToolButton;
class QProgressBar;
class QStackedWidget;

#include "account.h"
#include "models.h"

class DotCalendar;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    // UI builders
    QWidget* buildTopTitle();
    QWidget* buildMonthBar();
    QWidget* buildListPanel();
    QWidget* buildBottomBar();

    // Refresh
    void refreshDayList(const QDate& d);
    void refreshTodoList(const QDate& d);
    void refreshCalendarMarks();
    void refreshMonthSummary(const QDate& d);
    void checkBudgetWarning(const QDate& d);

    // Todo file helpers
    bool loadTodosForDate(const QDate& d);
    bool appendTodoToFile(const QDate& d, const Todo& td);
    bool setTodoDoneInFile(const QDate& d, int index, bool done);
    bool deleteTodoInFile(const QDate& d, int index);

    // Style
    void applyStyle();

private:
    // Data
    QDate currentDate;
    Account account;

    // Calendar
    DotCalendar *cal = nullptr;

    // Month bar
    QLabel *monthTitle = nullptr;

    // List panel
    QStackedWidget *stack = nullptr;

    // Account page (page 0)
    QLabel *sumLabel = nullptr;
    QListWidget *list = nullptr;

    // Todo page (page 1)
    QListWidget *todoList = nullptr;

    // Month summary
    QLabel *monthIncomeLabel = nullptr;
    QLabel *monthExpenseLabel = nullptr;
    QLabel *budgetLabel = nullptr;
    QProgressBar *budgetBar = nullptr;

    // Bottom bar
    QToolButton *btnBook = nullptr;
    QToolButton *btnPlus = nullptr;
    QToolButton *btnTodo = nullptr;

    //跨日顯示用容器（顯示當天所有覆蓋到這一天的 todo）
    QVector<Todo>  todos;
    QVector<bool>  todoDone;
    QVector<QDate> todoSrcDate;
    QVector<int>   todoSrcIndex;

};

#endif // MAINWINDOW_H
