#include "qoclbm_params.h"
#include "ui_qoclbm_params.h"

QOclBM_params::QOclBM_params(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QOclBM_params)
{
    ui->setupUi(this);

    ui->label_disp->setNum( ui->horizontalSlider_disp->value() );

    int winSize = ui->horizontalSlider_win->value();
    if(winSize%2 == 0)
        winSize++;

    ui->horizontalSlider_win->setValue(winSize);
    ui->label_disp->setNum( winSize );
}

QOclBM_params::~QOclBM_params()
{
    delete ui;
}

int QOclBM_params::getDisp()
{
    return ui->horizontalSlider_disp->value();
}

int QOclBM_params::getWinSize()
{
    return ui->horizontalSlider_win->value();
}

int QOclBM_params::getFilterMode()
{
    return ui->radioButton_basic->isChecked()?0:1;
}

void QOclBM_params::on_horizontalSlider_win_valueChanged(int value)
{
    if(value%2 == 0)
        value++;

    ui->horizontalSlider_win->setValue(value);
}
