#include "../Include/eventlogsortfilter.h"

EventLogSortFilter::EventLogSortFilter()
{
    setFilterKeyColumn(-1);
}

void EventLogSortFilter::applyFilter(QRegExp reg)
{
    reg.setCaseSensitivity(Qt::CaseInsensitive);
    setFilterRegExp(reg);
}

bool EventLogSortFilter::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QAbstractItemModel* model = sourceModel();
    QString line =QString("%1;%2;%3; %4")
            .arg(model->data(model->index(source_row,0,source_parent),Qt::DisplayRole).toString())
            .arg(model->data(model->index(source_row,2,source_parent),Qt::DisplayRole).toString())
            .arg(model->data(model->index(source_row,3,source_parent),Qt::DisplayRole).toString())
            .arg(model->data(model->index(source_row,4,source_parent),Qt::DisplayRole).toString());
    return line.contains(filterRegExp());
}
