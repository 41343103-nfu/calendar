#include "account.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>

// ===== 建構子 =====
Account::Account()
{
}

// ===== 新增一筆記帳 =====
void Account::addItem(const AccountItem &item)
{
    m_items.append(item);
}

// ===== 清空當日資料（切換日期時用）=====
void Account::clearDailyItems()
{
    m_items.clear();
}

// ===== 每日結算 =====
double Account::dailyIncome() const
{
    double sum = 0;
    for (const auto &item : m_items) {
        if (item.type == "income")
            sum += item.amount;
    }
    return sum;
}

double Account::dailyExpense() const
{
    double sum = 0;
    for (const auto &item : m_items) {
        if (item.type == "expense")
            sum += item.amount;
    }
    return sum;
}

double Account::dailyNet() const
{
    return dailyIncome() - dailyExpense();
}

// ===== 每月結算 =====
double Account::monthlyIncome(int year, int month)
{
    double total = 0;
    QDate date(year, month, 1);
    int days = date.daysInMonth();

    for (int d = 1; d <= days; ++d) {
        QDate current(year, month, d);
        if (loadFromFile(current)) {
            total += dailyIncome();
        }
    }
    return total;
}

double Account::monthlyExpense(int year, int month)
{
    double total = 0;
    QDate date(year, month, 1);
    int days = date.daysInMonth();

    for (int d = 1; d <= days; ++d) {
        QDate current(year, month, d);
        if (loadFromFile(current)) {
            total += dailyExpense();
        }
    }
    return total;
}

// ===== 預算 =====
void Account::setMonthlyBudget(double budget)
{
    m_monthlyBudget = budget;
}

double Account::getMonthlyBudget() const
{
    return m_monthlyBudget;
}

bool Account::isBudgetWarning(int year, int month) const
{
    if (m_monthlyBudget <= 0)
        return false;

    double expense = 0;
    Account temp = *this;
    expense = temp.monthlyExpense(year, month);

    return expense >= m_monthlyBudget * 0.8; // 80% 警告
}

// ===== 檔案路徑 =====
QString Account::filePath(const QDate &date) const
{
    QDir dir("data");
    if (!dir.exists())
        dir.mkpath(".");

    return QString("data/%1.json").arg(date.toString("yyyy-MM-dd"));
}

// ===== 讀檔 =====
bool Account::loadFromFile(const QDate &date)
{
    clearDailyItems();

    QFile file(filePath(date));
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonObject root = doc.object();
    QJsonArray arr = root["account"].toArray();

    for (const auto &v : arr) {
        QJsonObject obj = v.toObject();
        AccountItem item;
        item.type = obj["type"].toString();
        item.category = obj["category"].toString();
        item.amount = obj["amount"].toDouble();
        item.note = obj["note"].toString();
        m_items.append(item);
    }

    if (root.contains("monthly_budget"))
        m_monthlyBudget = root["monthly_budget"].toDouble();

    return true;
}

// ===== 存檔 =====
bool Account::saveToFile(const QDate &date) const
{
    QJsonArray arr;
    for (const auto &item : m_items) {
        QJsonObject obj;
        obj["type"] = item.type;
        obj["category"] = item.category;
        obj["amount"] = item.amount;
        obj["note"] = item.note;
        arr.append(obj);
    }

    QJsonObject root;
    root["account"] = arr;
    root["monthly_budget"] = m_monthlyBudget;

    QFile file(filePath(date));
    if (!file.open(QIODevice::WriteOnly))
        return false;

    file.write(QJsonDocument(root).toJson());
    file.close();
    return true;
}
