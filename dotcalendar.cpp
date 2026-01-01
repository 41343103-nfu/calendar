#include "dotcalendar.h"
#include <QPainter>

static const QColor BG("#0B0B0B");
static const QColor TEXT("#EDEDED");
static const QColor DIM("#555555");
static const QColor ACCENT("#F5A623");        // 今天橘色
static const QColor SELECTED(255,255,255,40); // ✅ 淺灰半透明

DotCalendar::DotCalendar(QWidget *parent)
    : QCalendarWidget(parent),
    chosen(QDate::currentDate())
{
    setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    setGridVisible(false);
    setNavigationBarVisible(false);
    setFirstDayOfWeek(Qt::Sunday);

    connect(this, &QCalendarWidget::clicked, this, [=](const QDate &d){
        chosen = d;
        update();
    });

    QFont f = font();
    f.setPointSize(12);
    setFont(f);
}

void DotCalendar::setMarkedDates(const QSet<QDate> &dates) {
    marked = dates;
    update();
}

QDate DotCalendar::chosenDate() const {
    return chosen.isValid() ? chosen : QDate::currentDate();
}

void DotCalendar::setChosenDate(const QDate &d) {
    chosen = d;
    update();
}

void DotCalendar::paintCell(QPainter *p, const QRect &r, QDate d) const {
    p->save();

    // 背景
    p->setPen(Qt::NoPen);
    p->setBrush(BG);
    p->drawRect(r);

    const bool isToday = (d == QDate::currentDate());
    const bool inMonth = (d.month() == monthShown() && d.year() == yearShown());
    const bool isChosen = (d == chosenDate()) && !isToday;

    auto drawPill = [&](const QColor& c){
        QRect pill = r.adjusted(int(r.width()*0.20), int(r.height()*0.18),
                                -int(r.width()*0.20), -int(r.height()*0.18));
        p->setBrush(c);
        p->setPen(Qt::NoPen);
        p->drawRoundedRect(pill, pill.height()/2.0, pill.height()/2.0);
    };

    // 今天橘色；選到別天灰色
    if (isToday) drawPill(ACCENT);
    else if (isChosen) drawPill(SELECTED);

    // 文字顏色
    if (!inMonth) p->setPen(DIM);
    else if (isToday || isChosen) p->setPen(Qt::white);
    else p->setPen(TEXT);

    p->drawText(r.adjusted(0, -2, 0, 0), Qt::AlignCenter, QString::number(d.day()));

    // 白點：當天有「記帳或待辦」就顯示
    if (marked.contains(d)) {
        p->setBrush(TEXT);
        p->setPen(Qt::NoPen);
        QPoint center(r.center().x(), r.bottom() - int(r.height()*0.18));
        int rad = qMax(2, r.width()/18);
        p->drawEllipse(center, rad, rad);
    }

    p->restore();
}
