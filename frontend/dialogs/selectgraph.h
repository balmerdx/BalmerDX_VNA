#ifndef SELECTGRAPH_H
#define SELECTGRAPH_H

#include <QGroupBox>

class QComboBox;
class QLabel;
class QCheckBox;

class LCInfoWidget;

enum class MathModel
{
    BAD,

    //Данные без калибровки
    RAW,

    //Предполагаем, что деталь является двухполюсником, и  подключенна к S11.
    //Для этой модели достаточно данныс с RX входа.
    S11,

    //Деталь является четырехполюсником,
    //симметричным и пассивныс.
    //Для этой модели достаточно данных с RX и TX входа. Один проход.
    RECIPROCAL_SYMMETRICAL,

    //Полный случай, когда вычисляются все 4 S-параметра.
    //Для этой модели необходимо снять данные с RX и TX входов.
    //Потом перевернуть деталь и опять снять данные.
    FULL
};

enum class GraphType
{
    BAD,
    ABS_S11,
    ABS_S11_S21,
    ABS_S12_S22,
    S11,
    S21,
    S12,
    S22,

    DB_S21,
    DB_S12,
    //Комплексное сопротивление (в предположении, что подключили двухполюсник к RX входу)
    Z_S11,
    Z_S22,

    //Предполагаем, что симметричную деталь подключили между RX и TX
    Z_RX_TX,
};

class SelectGraph : public QGroupBox
{
    Q_OBJECT
public:
    explicit SelectGraph(QWidget *parent, LCInfoWidget* lcinfo);

    GraphType graphType();
    MathModel mathModel();

    bool isErrorsVisible();

    void setFreq(double freq);
    void setValuesPattern(QStringList pattern, bool isZ = false);
    void setValues(QList<double> values, float F);
signals:

public slots:
    void onGraphTypeChanged(int);
    void onMathModelChanged(int);

    void onSParamChanged();

    void onErrorsVisible(int);
protected:
    void updateGraphType();
protected:
    bool _inSetData;
    QComboBox* _comboModelType;
    QComboBox* _comboGraphType;
    QLabel* _legend;
    QCheckBox* _isErrorsVisible;
    QLabel* _freq;

    MathModel _prevModel;
    GraphType _prevGraph;

    QStringList _pattern;
    bool _isZPattern;

    LCInfoWidget* _lcinfo;
};

extern SelectGraph* g_graphSettings;

#endif // SELECTGRAPH_H
