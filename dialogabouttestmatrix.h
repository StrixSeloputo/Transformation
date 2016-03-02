#ifndef DIALOGABOUTTESTMATRIX_H
#define DIALOGABOUTTESTMATRIX_H

#define SIZE                    9
#define ORD                     3

#include <QDialog>

namespace Ui {
class DialogAboutTestMatrix;
}

class DialogAboutTestMatrix : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAboutTestMatrix(QWidget *parent = 0);
    ~DialogAboutTestMatrix();
signals:
    void readyTestMatrix(double *U);
private:
    Ui::DialogAboutTestMatrix *ui;
    double U[SIZE];
private slots:
    void clearEnter();
    void apply();
    void accept();
};

#endif // DIALOGABOUTTESTMATRIX_H
