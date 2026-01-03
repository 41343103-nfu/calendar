#include "mainwindow.h"
#include "dotcalendar.h"
#include "addentrydialog.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QToolButton>
#include <QFrame>
#include <QFile>
<<<<<<< HEAD

#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>
=======
#include <QTextStream>
#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QPushButton>

>>>>>>> b4c91a3f047dd1b2106170cd6707e0ec599d6f48

static const QColor BG("#0B0B0B");
static const QColor PANEL("#141414");
static const QColor TEXT("#EDEDED");
static const QColor DIM("#9A9A9A");
static const QColor ACCENT("#F5A623");

static QString monthTitleZh(int y, int m){
    return QString("%1年%2月").arg(y).arg(m);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Calendar Mock");
    setMinimumSize(390, 780);

    auto *root = new QWidget(this);
    auto *v = new QVBoxLayout(root);
    v->setContentsMargins(14,14,14,14);
    v->setSpacing(10);

    v->addWidget(buildTopTitle());
    v->addWidget(buildMonthBar());

    cal = new DotCalendar(this);
    v->addWidget(cal, 1);

    v->addWidget(buildListPanel(), 0);
    v->addWidget(buildBottomBar(), 0);

    setCentralWidget(root);

    applyStyle();
<<<<<<< HEAD

    // ✅ 初始化日期 + 載入今天記帳
    currentDate = QDate::currentDate();
    account.loadFromFile(currentDate);

=======
    list->setContextMenuPolicy(Qt::CustomContextMenu); // 允許清單跳出右鍵選單
>>>>>>> b4c91a3f047dd1b2106170cd6707e0ec599d6f48
    monthTitle->setText(monthTitleZh(cal->yearShown(), cal->monthShown()));

    connect(cal, &QCalendarWidget::clicked, this, [=](const QDate &d){
        currentDate = d;
        account.loadFromFile(d);
        refreshDayList(d);
    });

    connect(cal, &QCalendarWidget::currentPageChanged, this, [=](int y, int m){
        monthTitle->setText(monthTitleZh(y, m));
        refreshCalendarMarks(); // ✅ 翻月時更新點點
    });
    //在清單上點右鍵時發動
    connect(list, &QListWidget::customContextMenuRequested, this, [=](const QPoint &pos){
        QListWidgetItem *item = list->itemAt(pos);
        if (!item) return;
        QMenu menu(this);
        QAction *delAct = menu.addAction("刪除提醒");
        QAction *editAct = menu.addAction("修改提醒");
        QAction *selected = menu.exec(list->mapToGlobal(pos));

        if (selected == delAct) {
            QString title = item->text().split(" (")[0].replace(" [待辦] ", "");

            //移除項目
            for (int i = 0; i < todos.size(); ++i) {
                if (todos[i].title == title) {
                    todos.removeAt(i);
                    break;
                }
            }

            //同步
            saveTodosToFile();      // 同步更新檔案
            refreshCalendarMarks(); // 重整日曆小白點和下面的清單
            refreshDayList(cal->chosenDate());}
        else if (selected == editAct) {
                QString title = item->text().split(" (")[0].replace(" [待辦] ", "");
                if (!item) return;
                QString id = item->data(Qt::UserRole).toString();

                Todo td;
                bool found = false;
                for (const auto &t : todos) {
                 if (t.id == id) {
                        td = t;
                        found = true;
                        break;
                    }
                }
            if (!found) return; // 沒找到就不開啟編輯

                AddEntryDialog dlg(cal->chosenDate(), this);
                dlg.setTodo(td);
                connect(&dlg, &AddEntryDialog::savedTodo, this, [=](const Todo& td){

                    for (int i = 0; i < todos.size(); ++i) {
                        if (todos[i].id == td.id) {
                            todos[i] = td;
                            saveTodosToFile();
                            return;
                        }
                    }
                    todos.push_back(td);
                    saveTodosToFile();
                });
                if (dlg.exec() == QDialog::Accepted) {
                    refreshCalendarMarks();
                    refreshDayList(cal->chosenDate());
                }
            }
        });

<<<<<<< HEAD
    refreshDayList(currentDate);
=======
    refreshDayList(QDate::currentDate());
    loadTodosFromFile(); // 啟動時自動從todos.txt讀取資料
>>>>>>> b4c91a3f047dd1b2106170cd6707e0ec599d6f48
    refreshCalendarMarks();

}

QWidget* MainWindow::buildTopTitle() {
    auto *w = new QWidget(this);
    auto *h = new QHBoxLayout(w);
    h->setContentsMargins(0,0,0,0);

    auto *title = new QLabel("行事曆", w);
    title->setObjectName("topTitle");
    title->setAlignment(Qt::AlignCenter);

    h->addStretch(1);
    h->addWidget(title);
    h->addStretch(1);

    return w;
}

QWidget* MainWindow::buildMonthBar() {
    auto *w = new QWidget(this);
    auto *h = new QHBoxLayout(w);
    h->setContentsMargins(0,0,0,0);

    auto mkBtn = [&](const QString& t){
        auto *b = new QToolButton(w);
        b->setText(t);
        b->setFixedSize(34, 28);
        return b;
    };

    auto *prev = mkBtn("‹");
    auto *next = mkBtn("›");
    monthTitle = new QLabel(w);
    monthTitle->setAlignment(Qt::AlignCenter);
    monthTitle->setObjectName("monthTitle");

    h->addWidget(prev);
    h->addStretch(1);
    h->addWidget(monthTitle);
    h->addStretch(1);
    h->addWidget(next);

    connect(prev, &QToolButton::clicked, this, [=]{ cal->showPreviousMonth(); });
    connect(next, &QToolButton::clicked, this, [=]{ cal->showNextMonth(); });

    return w;
}

QWidget* MainWindow::buildListPanel() {
    auto *panel = new QFrame(this);
    panel->setObjectName("listPanel");
    auto *v = new QVBoxLayout(panel);
    v->setContentsMargins(12,10,12,10);
    v->setSpacing(8);

    sumLabel = new QLabel("支出:0", panel);
    sumLabel->setObjectName("sumLabel");
    v->addWidget(sumLabel);

    list = new QListWidget(panel);
    list->setObjectName("expenseList");
    v->addWidget(list);

    // ✅ 右鍵刪除單筆記帳（不影響 ToDo）
    list->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(list, &QListWidget::customContextMenuRequested, this, [=](const QPoint &pos){
        auto *it = list->itemAt(pos);
        if (!it) return;

        int idx = it->data(Qt::UserRole).toInt();

        QMenu menu;
        QAction *del = menu.addAction("刪除這筆記帳");
        QAction *chosen = menu.exec(list->viewport()->mapToGlobal(pos));
        if (chosen != del) return;

        if (QMessageBox::question(this, "刪除", "確定刪除這筆記帳？") != QMessageBox::Yes)
            return;

        if (!account.removeAt(idx)) return;

        if (!account.saveToFile(currentDate)) {
            QMessageBox::warning(this, "存檔失敗", "刪除後無法寫入檔案 data/...");
            return;
        }

        refreshDayList(currentDate);
        refreshCalendarMarks();
    });

    return panel;
}

QWidget* MainWindow::buildBottomBar() {
    auto *w = new QWidget(this);
    auto *h = new QHBoxLayout(w);
    h->setContentsMargins(0,8,0,0);
    h->setSpacing(10);

    auto mk = [&](const QString& t){
        auto *b = new QToolButton(w);
        b->setText(t);
        b->setToolButtonStyle(Qt::ToolButtonTextOnly);
        b->setFixedHeight(44);
        return b;
    };

    btnBook = mk("帳本");
    btnPlus = mk("+");
    btnTodo = mk("待辦事項");
    btnPlus->setObjectName("plusBtn");

    h->addWidget(btnBook, 1);
    h->addWidget(btnPlus, 1);
    h->addWidget(btnTodo, 1);

<<<<<<< HEAD
    // ✅ 設定預算（用輸入框先做完功能）
    connect(btnBook, &QToolButton::clicked, this, [=]{
        bool ok = false;
        double b = QInputDialog::getDouble(
            this,
            "設定預算",
            "本月預算：",
            account.getMonthlyBudget(),
            0, 1e9, 0,
            &ok
            );
        if (!ok) return;

        account.setMonthlyBudget(b);
        if (!account.saveToFile(currentDate)) {
            QMessageBox::warning(this, "存檔失敗", "預算無法寫入檔案 data/...");
            return;
        }

        QMessageBox::information(this, "完成", QString("已設定本月預算：%1").arg(b));
        checkBudgetWarning(currentDate);
    });

    // ✅ 新增記帳：修正「存到別天/像沒反應」
=======
    connect(btnTodo, &QToolButton::clicked, this, &MainWindow::showAllTodos);
>>>>>>> b4c91a3f047dd1b2106170cd6707e0ec599d6f48
    connect(btnPlus, &QToolButton::clicked, this, [=]{
        QDate d = cal->chosenDate();

        // ✅ 同步 currentDate，並先載入當天檔案（避免覆蓋）
        currentDate = d;
        account.loadFromFile(d);

        AddEntryDialog dlg(d, this);

        connect(&dlg, &AddEntryDialog::savedExpenseIncome, this, [=](const AccountItem& item){
            account.addItem(item);

            // ✅ 存檔用 d（跟 dialog 日期一致）
            if (!account.saveToFile(d)) {
                QMessageBox::warning(this, "存檔失敗", "無法寫入 data/ 資料夾（權限或路徑問題）。");
                return;
            }

            refreshDayList(d);
            refreshCalendarMarks();
            checkBudgetWarning(d);
        });

        connect(&dlg, &AddEntryDialog::savedTodo, this, [=](const Todo& td){
            todos.push_back(td);
            refreshCalendarMarks();
            saveTodosToFile();
        });

        dlg.exec();
    });

    return w;
}

void MainWindow::refreshDayList(const QDate&) {
    if (!list || !sumLabel) return;

    list->clear();

    double sumExpense = account.dailyExpense();

    int idx = 0;
    for (const auto &item : account.getItems()) {
        QString sign = (item.type == "income") ? "收入" : "支出";

        auto *it = new QListWidgetItem(QString("%1\n%2  %3")
                                           .arg(item.category)
                                           .arg(sign)
                                           .arg(item.amount));
        it->setData(Qt::UserRole, idx); // ✅ 對應 m_items index
        list->addItem(it);
        idx++;
    }

    sumLabel->setText(QString("支出:%1").arg(sumExpense));
    // *顯示待辦事項
    for (const auto &td : todos) {
        if (td.start.date() == d) {
            QString timeStr = td.allDay ? "全天" : td.start.toString("hh:mm");
            auto *item = new QListWidgetItem(QString(" [待辦] %1 (%2)").arg(td.title).arg(timeStr));
            item->setData(Qt::UserRole, td.id);
            list->addItem(item);

        }
    }
}

void MainWindow::refreshCalendarMarks() {
    QSet<QDate> marks;

    QDate first(cal->yearShown(), cal->monthShown(), 1);
    int days = first.daysInMonth();

    for (int d = 1; d <= days; ++d) {
        QDate date(first.year(), first.month(), d);
        QString path = QString("data/%1.json").arg(date.toString("yyyy-MM-dd"));
        if (QFile::exists(path))
            marks.insert(date);
    }

    cal->setMarkedDates(marks);
}

void MainWindow::checkBudgetWarning(const QDate& d) {
    double budget = account.getMonthlyBudget();
    if (budget <= 0) return;

    double monthExpense = account.monthlyExpense(d.year(), d.month());
    if (monthExpense >= budget * 0.8) {
        QMessageBox::warning(this, "預算提醒",
                             QString("本月支出已達 %1 / %2（80%%）")
                                 .arg(monthExpense)
                                 .arg(budget));
    }
}

void MainWindow::applyStyle() {
    qApp->setStyleSheet(QString(R"(
        QWidget { background: %1; color: %2; }

        QLabel#topTitle { font-size: 18px; font-weight: 700; color: %2; padding: 6px 0px; }
        QLabel#monthTitle { font-size: 20px; font-weight: 700; color: %2; }

        QToolButton { border: 1px solid #2A2A2A; border-radius: 12px; background: #121212; color: %2; padding: 6px; }
        QToolButton:hover { background: #1A1A1A; }

        QToolButton#plusBtn { font-size: 18px; font-weight: 700; background: #EDEDED; color: #111; border: none; }

        QFrame#listPanel { background: %3; border: 1px solid #1E1E1E; border-radius: 14px; }
        QLabel#sumLabel { color: %2; font-size: 14px; padding: 6px 2px; }

        QListWidget { background: transparent; border: none; }
        QListWidget::item { padding: 10px; border-bottom: 1px solid #1E1E1E; color: %2; }
    )").arg(BG.name(), TEXT.name(), PANEL.name()));
}

void MainWindow::saveTodosToFile() {
    QFile file("todos.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const auto &td : todos) {
            out << td.id << "|"
                << td.title << "|"
                << td.start.toString(Qt::ISODate) << "|"
                << td.end.toString(Qt::ISODate) << "|"
                << (td.allDay ? "1" : "0") << "\n";
        }
        file.close();
    }
}

void MainWindow::loadTodosFromFile() {
    QFile file("todos.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split("|");
        if (parts.size() >= 5) {
            Todo td;
            td.id    = parts[0];
            td.title = parts[1];
            td.start = QDateTime::fromString(parts[2], Qt::ISODate);
            td.end   = QDateTime::fromString(parts[3], Qt::ISODate);
            td.allDay= (parts[4] == "1");
            todos.push_back(td);
        }
    }
    file.close();
}

void MainWindow::showAllTodos() {
    QDialog dlg(this);
    dlg.setWindowTitle("所有待辦事項");
    dlg.setMinimumSize(350, 500);

    auto *v = new QVBoxLayout(&dlg);
    auto *allList = new QListWidget(&dlg);

    for (const auto &td : todos) {
        QString timeStr = td.allDay ? "全天" : td.start.toString("yyyy/MM/dd hh:mm");
        allList->addItem(QString("[%1] %2").arg(timeStr).arg(td.title));
    }

    v->addWidget(new QLabel("完整待辦清單：", &dlg));
    v->addWidget(allList);

    auto *btnClose = new QPushButton("關閉", &dlg);
    connect(btnClose, &QPushButton::clicked, &dlg, &QDialog::accept);
    v->addWidget(btnClose);

    dlg.exec();
}
