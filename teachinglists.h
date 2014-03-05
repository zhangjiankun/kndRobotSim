#ifndef TEACHINGLISTS_H
#define TEACHINGLISTS_H
#include<QString>
#include<QStringList>
#include<QStringListModel>

class TeachingLists
{
public:
    TeachingLists(const QString &filename);
    ~TeachingLists();

    QString sourceFile;//存储运行指令的源文件
    QStringList list; //运行指令队列
    QStringListModel *listModel;
};

#endif // TEACHINGLISTS_H
