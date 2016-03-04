#ifndef DIALOGABOUTTESTMATRIX_H
#define DIALOGABOUTTESTMATRIX_H

#include <QDialog>

#include "constdef.h"

namespace Ui {
class DialogAboutTestMatrix;
}

class DialogAboutTestMatrix : public QDialog
{
    Q_OBJECT

private:
    Ui::DialogAboutTestMatrix  *_ui;
    double                      _U[SIZE];
public:
    explicit DialogAboutTestMatrix(QWidget *parent = 0);
    ~DialogAboutTestMatrix();
signals:
    void readyTestMatrix(double *U);
private slots:
    void clearEnter();
    void apply();
    void accept();
};

#endif // DIALOGABOUTTESTMATRIX_H
