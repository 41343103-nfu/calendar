#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>
#include <QVector>
#include <QDate>

struct AccountItem
{
    QString type;      // "income" or "expense"
    QString category;  // food, transport, etc.
    double amount;
    QString note;
};

class Account
{
public:
    Account();

    // ===== 記帳操作 =====
    void addItem(const AccountItem &item);
    void clearDailyItems();

    // ===== 每日結算 =====
    double dailyIncome() const;
    double dailyExpense() const;
    double dailyNet() const;

    // ===== 每月結算 =====
    double monthlyIncome(int year, int month);
    double monthlyExpense(int year, int month);

    // ===== 預算設定 =====
    void setMonthlyBudget(double budget);
    double getMonthlyBudget() const;
    bool isBudgetWarning(int year, int month) const;

    // ===== 資料存取 =====
    bool loadFromFile(const QDate &date);
    bool saveToFile(const QDate &date) const;

private:
    QVector<AccountItem> m_items;
    double m_monthlyBudget = 0.0;

    QString filePath(const QDate &date) const;
};

#endif // ACCOUNT_H
