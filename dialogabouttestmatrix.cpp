
#include <math.h>

#include <QMessageBox>

#include "dialogabouttestmatrix.h"
#include "helpfunction.h"
#include "ui_dialogabouttestmatrix.h"

DialogAboutTestMatrix::DialogAboutTestMatrix(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::DialogAboutTestMatrix)
{
    _ui->setupUi(this);
    for (int i = 0; i < SIZE; i++) _U[i] = 0;
    for (int i = 0; i < ORD; i++) _U[i+ORD*i] = 1; // E-matrix
    connect(_ui->applyButton, SIGNAL(clicked()), this, SLOT(apply()));
    connect(_ui->clearButton, SIGNAL(clicked()), this, SLOT(clearEnter()));
}

DialogAboutTestMatrix::~DialogAboutTestMatrix()
{
    delete _ui;
}

void DialogAboutTestMatrix::accept()
{
    apply();
    emit readyTestMatrix(_U);
    close();
}
void DialogAboutTestMatrix::apply()
{
    double W[SIZE] = { 0 };
    for (int i = 0; i < ORD; i++)
        W[i*ORD+i] = 1;
    double V[SIZE] = { 0 };
    double ox = 1, oy = 1, oz = 1;
    double angle = 0;
    if(_ui->homothetyRadioButton->isChecked())
    {
        qDebug("Homothety was selected");
        if(_ui->homothetyOxCheckBox->isChecked())
        {
            ox = _ui->homothetyOxEdit->text().toInt();
            if (ox == 0)
            {
                QMessageBox::about(0, QObject::tr("Коэффициент подобия равен нулю!"),
                                     QObject::tr("Коэффициент подобия по оси OX равен нулю!"));
                return;
            }
            if(_ui->homothetyCompressionOxCheckBox->isChecked()) ox = 1/ox;
        }
        if(_ui->homothetyOyCheckBox->isChecked())
        {
            oy = _ui->homothetyOyEdit->text().toInt();
            if (oy == 0)
            {
                QMessageBox::about(0, tr("Коэффициент подобия равен нулю!"),
                                     tr("Коэффициент подобия по оси OY равен нулю!"));
                return;
            }
            if(_ui->homothetyCompressionOyCheckBox->isChecked()) oy = 1/oy;
        }
        if(_ui->homothetyOzCheckBox->isChecked())
        {
            oy = _ui->homothetyOzEdit->text().toInt();
            if (oy == 0)
            {
                QMessageBox::about(0, tr("Коэффициент подобия равен нулю!"),
                                     tr("Коэффициент подобия по оси OZ равен нулю!"));
                return;
            }
            if(_ui->homothetyCompressionOzCheckBox->isChecked()) oz = 1/oz;
        }
    }
    if(_ui->symmetryRadioButton->isChecked())
    {
        qDebug("Symmetry was selected");
        if(_ui->symmetryOxCheckBox->isChecked()) ox *= -1;
        if(_ui->symmetryOyCheckBox->isChecked()) oy *= -1;
        if(_ui->symmetryOzCheckBox->isChecked()) oz *= -1;
    }

    for (int i = 0; i < ORD; i++)
    {
        W[i*ORD] *= ox;
        W[i*ORD+1] *= oy;
        W[i*ORD+2] *= oz;
    }

    if(_ui->turningRadioButton->isChecked())
    {
        qDebug("Turning was selected");
        angle = _ui->turningAngleEdit->text().toDouble(); //в градусах
        angle = (angle*M_PI)/180; //в радианах
        // вокруг оси Oz
        if (_ui->turningOxyRadioButton->isChecked()) {
            W[0] = W[ORD+1] = cos(angle);
            W[1] = -sin(angle);
            W[ORD] = sin(angle);
            W[2*ORD+2] = 1;
        }
        // вокруг оси Oy
        if (_ui->turningOxzRadioButton->isChecked()) {
            W[0] = W[2*ORD+2] = cos(angle);
            W[2*ORD] = -sin(angle);
            W[ORD+2] = sin(angle);
            W[ORD+1] = 1;
        }
        // вокруг оси Ox
        if (_ui->turningOyzRadioButton->isChecked()) {
            W[2*ORD+2] = W[ORD+1] = cos(angle);
            W[ORD+2] = -sin(angle);
            W[2*ORD+1] = sin(angle);
            W[0] = 1;
        }
    }
    // V = W*_U поскольку применяется сначала _U, потом W
    for (int i = 0; i < ORD; i++)
        for (int j = 0; j < ORD; j++)
            for (int k = 0; k < ORD; k++)
                V[i*ORD+j] = W[i*ORD+k]*_U[k*ORD+j];
    // _U = V
    for(int i = 0; i < SIZE; i++) _U[i] = V[i];
    QMessageBox::about(
        0, tr("Test Matrix"),
        HelpFunction::showMatrix(_U);
    );
    qDebug("Ready Test Matrix");
}

void DialogAboutTestMatrix::clearEnter()
{
    _ui->homothetyRadioButton->setChecked(false);
    _ui->homothetyOxCheckBox->setChecked(false);
    _ui->homothetyOyCheckBox->setChecked(false);
    _ui->homothetyOzCheckBox->setChecked(false);
    _ui->homothetyCompressionOxCheckBox->setChecked(false);
    _ui->homothetyCompressionOyCheckBox->setChecked(false);
    _ui->homothetyCompressionOzCheckBox->setChecked(false);
    _ui->homothetyOxEdit->setText("1");
    _ui->homothetyOyEdit->setText("1");
    _ui->homothetyOzEdit->setText("1");

    _ui->symmetryRadioButton->setChecked(false);
    _ui->symmetryOxCheckBox->setChecked(false);
    _ui->symmetryOyCheckBox->setChecked(false);
    _ui->symmetryOzCheckBox->setChecked(false);

    _ui->translationRadioButton->setChecked(false);
    _ui->translationXEdit->setText("0");
    _ui->translationYEdit->setText("0");
    _ui->translationZEdit->setText("0");

    _ui->turningRadioButton->setChecked(false);
    _ui->turningOxyRadioButton->setChecked(false);
    _ui->turningOxzRadioButton->setChecked(false);
    _ui->turningOyzRadioButton->setChecked(false);
    _ui->turningAngleEdit->setText("0");
}
