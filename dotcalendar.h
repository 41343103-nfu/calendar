#pragma once
#include <QCalendarWidget>
#include <QSet>
#include <QDate>

class DotCalendar : public QCalendarWidget {
    Q_OBJECT
public:
    explicit DotCalendar(QWidget *parent = nullptr);

    void setMarkedDates(const QSet<QDate>& dates);

    QDate chosenDate() const;
    void setChosenDate(const QDate& d);

protected:
    void paintCell(QPainter *painter, const QRect &rect, QDate date) const override;

private:
    QSet<QDate> marked;

    // 今天橘色，選到其他日期灰色
    QDate chosen;
};
