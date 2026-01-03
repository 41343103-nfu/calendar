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

#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>

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

    // ✅ 初始化日期 + 載入今天記帳
    currentDate = QDate::currentDate();
    account.loadFromFile(currentDate);

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

    refreshDayList(currentDate);
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
