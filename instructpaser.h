#ifndef INSTRUCTPASER_H
#define INSTRUCTPASER_H
#include<QStringList>
#include<QDebug>
class InstructPaser
{
public:
    InstructPaser();
    static unsigned int paser(const QStringList &inLists, QStringList &outLists)
    {
        outLists = inLists;
        return 0;
    }
};

#endif // INSTRUCTPASER_H
