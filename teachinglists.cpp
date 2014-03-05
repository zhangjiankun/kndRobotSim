#include "teachinglists.h"
#include <QStringList>
#include <QtCore>
TeachingLists::TeachingLists(const QString &filename)
{
    if (NULL == filename)
    {
    //open file ，and save
    }
    listModel = new QStringListModel;

}

TeachingLists::~TeachingLists()
{
    delete listModel;
}
