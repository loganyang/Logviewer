#ifndef EVENTLOGSORTFILTER_H
#define EVENTLOGSORTFILTER_H

#include <QSortFilterProxyModel>
#include <QRegExp>
class EventLogSortFilter : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    EventLogSortFilter();
    void applyFilter(QRegExp reg);
public slots:

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};

#endif // EVENTLOGSORTFILTER_H
