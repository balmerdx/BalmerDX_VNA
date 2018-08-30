#ifndef DIALOGSELECTFREQUENCY_H
#define DIALOGSELECTFREQUENCY_H

#include <QDialog>

class QFormLayout;
class QComboBox;
class QDoubleSpinBox;
class QSpinBox;

class DialogSelectFrequency : public QWidget
{
    Q_OBJECT
public:
    explicit DialogSelectFrequency(QWidget *parent = 0);

    ~DialogSelectFrequency();

    //Записывает наредактированне данные в g_project
    void updateProject();
signals:

public slots:
    void onFreqTypeChanged(int);
protected:
protected:
    QFormLayout* topLayout;
    QComboBox* comboFreqType;
    QDoubleSpinBox* spinFrequencyMin;
    QDoubleSpinBox* spinFrequencyMax;
    QSpinBox* spinSamplesCount;

    QDoubleSpinBox* spinFrequencyAdditional;
};

#endif // DIALOGSELECTFREQUENCY_H
