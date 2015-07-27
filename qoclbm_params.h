#ifndef QOCLBM_PARAMS_H
#define QOCLBM_PARAMS_H

#include <QWidget>

namespace Ui {
class QOclBM_params;
}

class QOclBM_params : public QWidget
{
    Q_OBJECT

public:
    explicit QOclBM_params(QWidget *parent = 0);
    ~QOclBM_params();

    int getDisp();
    int getWinSize();
    int getFilterMode();

private slots:
    void on_horizontalSlider_win_valueChanged(int value);

private:
    Ui::QOclBM_params *ui;
};

#endif // QOCLBM_PARAMS_H
