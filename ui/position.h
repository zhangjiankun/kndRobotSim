#ifndef POSITION_H
#define POSITION_H

#include <QDialog>

namespace Ui {
class position;
}

class position : public QDialog
{
    Q_OBJECT
    
public:
    explicit position(QWidget *parent = 0);
    ~position();
    
private:
    Ui::position *ui;
};

#endif // POSITION_H
