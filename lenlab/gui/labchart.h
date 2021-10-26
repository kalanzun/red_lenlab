#ifndef GUI_LABCHART_H
#define GUI_LABCHART_H

#include "model/lenlab.h"

#include <QWidget>
#include <QtCharts>

namespace gui {

namespace Ui {
class LabChart;
}

class LabChart : public QWidget
{
    Q_OBJECT

    QPointer< QChart > m_chart;
    QList< QPointer< QLineSeries > > m_series;

public:
    explicit LabChart(QWidget *parent = nullptr);
    ~LabChart();

    QPointer< QChart > chart() const;
    QList< QPointer< QLineSeries > > series() const;

    void setLabelX(QString text);
    void setLabelY(QString text);
    void setLabelY2(QString text);

    void addSeries(QLineSeries *series);
    void addAxis(QAbstractAxis *axis, Qt::Alignment alignment);
    void createDefaultAxes();

    void replace(model::pSeries const & series);
    void appendLast(model::pSeries const & series);
    void setRange(model::pSeries const & series);

    void setChannelVisible(unsigned int channel, bool visible);

    void print(QString filename);

private:
    Ui::LabChart *ui;
};


} // namespace gui
#endif // GUI_LABCHART_H
