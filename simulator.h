#ifndef SIMULATOR_H
#define SIMULATOR_H
#include<QStringList>
class QTimer;
class Simulator
{
public:
    enum STATE{ IDLE, SHIELD_SPINBOX_SIG };
    Simulator();

signals:
    void sigModelChanged();
public slots:
    void startSimulation();
    void stopSimulation();
    void simulateLists();

private slots:


private:
    QStringList teachingList;
    unsigned long m_simTimeIntetvalM;
    QTimer *m_innerTimer;
    STATE m_state;
};

#endif // SIMULATOR_H
