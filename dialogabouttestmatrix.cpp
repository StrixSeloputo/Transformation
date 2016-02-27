
#include <math.h>

#include <QMessageBox>

#include "dialogabouttestmatrix.h"
#include "ui_dialogabouttestmatrix.h"

DialogAboutTestMatrix::DialogAboutTestMatrix(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAboutTestMatrix)
{
    ui->setupUi(this);
    U = new double [SIZE];
    for (int i = 0; i < SIZE; i++) U[i] = 0;
    for (int i = 0; i < ORD; i++) U[i+ORD*i] = 1; // E-matrix
    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(apply()));
    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clearEnter()));
}

DialogAboutTestMatrix::~DialogAboutTestMatrix()
{
    delete ui;
    delete []U;
}

void DialogAboutTestMatrix::accept()
{
    apply();
    emit readyTestMatrix(U);
    close();
}
void DialogAboutTestMatrix::apply()
{
    double W[SIZE] = {  1, 0, 0,
                        0, 1, 0,
                        0, 1, 0};
    if(ui->homothetyRadioButton->isChecked()) {
        qDebug("homothety is selected");
        double ox = 1, oy = 1;
        if(ui->homothetyOxCheckBox->isChecked()) {
            ox = ui->homothetyOxEdit->text().toInt();
            if (ox == 0) {
                QMessageBox::about(0, QObject::tr("Коэффициент подобия равен нулю!"),
                                     QObject::tr("Коэффициент подобия по оси OX равен нулю!"));
                return;
            }
            if(ui->homothetyCompressionOxCheckBox->isChecked()) ox = 1/ox;
        }
        if(ui->homothetyOyCheckBox->isChecked()) {
            oy = ui->homothetyOyEdit->text().toInt();
            if (oy == 0) {
                QMessageBox::about(0, tr("Коэффициент подобия равен нулю!"),
                                     tr("Коэффициент подобия по оси OY равен нулю!"));
                return;
            }
            if(ui->homothetyCompressionOyCheckBox->isChecked()) oy = 1/oy;
        }
        W[0] *= ox; W[3] *= ox;
        W[1] *= oy; W[4] *= oy;
    }
    if(ui->symmetryRadioButton->isChecked()) {
        qDebug("symmetry is selected");
        int ox = 1, oy = 1;
        if(ui->symmetryOxCheckBox->isChecked()) ox = -1;
        if(ui->symmetryOyCheckBox->isChecked()) oy = -1;
        W[0] *= ox; W[3] *= ox;
        W[1] *= oy; W[4] *= oy;
    }
    if(ui->translationRadioButton->isChecked()) {
        qDebug("translation is selected");
        double x, y;
        x = ui->translationXEdit->text().toInt();
        y = ui->translationYEdit->text().toInt();
        W[2] = x;
        W[5] = y;
    }
    if(ui->turningRadioButton->isChecked()) {
        qDebug("turning is selected");
        double angle = 0;
        angle = ui->turningAngleEdit->text().toDouble(); //в градусах
        angle = (angle*M_PI)/180; //в радианах
        W[0] = cos(angle);
        W[1] = -sin(angle);
        W[3] = sin(angle);
        W[4] = cos(angle);
    }
    double V[6];
    V[0] = W[0]*U[0] + W[1]*U[3];
    V[1] = W[0]*U[1] + W[1]*U[4];
    V[2] = W[0]*U[2] + W[1]*U[5] + W[2];
    V[3] = W[3]*U[0] + W[4]*U[3];
    V[4] = W[3]*U[1] + W[4]*U[4];
    V[5] = W[3]*U[2] + W[4]*U[5] + W[5];
    for(int i = 0; i < 6; i++) U[i] = V[i];
    QMessageBox::about(
        0, tr("Test Matrix"),
        QString::asprintf(
                    "%.2lf\t%.2lf\t%.2lf\n%.2lf\t%.2lf\t%.2lf\n0.00\t0.00\t1.00\n", U[0], U[1], U[2], U[3], U[4], U[5]
        )
    );
    qDebug("ready Test Matrix");
}

void DialogAboutTestMatrix::clearEnter()
{
    ui->homothetyRadioButton->setChecked(false);
    ui->homothetyOxCheckBox->setChecked(false);
    ui->homothetyOyCheckBox->setChecked(false);
    ui->homothetyCompressionOxCheckBox->setChecked(false);
    ui->homothetyCompressionOyCheckBox->setChecked(false);
    ui->homothetyOxEdit->setText("1");
    ui->homothetyOyEdit->setText("1");

    ui->symmetryRadioButton->setChecked(false);
    ui->symmetryOxCheckBox->setChecked(false);
    ui->symmetryOyCheckBox->setChecked(false);

    ui->translationRadioButton->setChecked(false);
    ui->translationXEdit->setText("0");
    ui->translationYEdit->setText("0");

    ui->turningRadioButton->setChecked(false);
    ui->turningAngleEdit->setText("0");
}
