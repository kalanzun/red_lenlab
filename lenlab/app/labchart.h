#ifndef APP_LABCHART_H
#define APP_LABCHART_H

#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QWidget>

namespace model {
class Component;
struct Sample;
class Waveform;
}

namespace app {

namespace Ui { class LabChart; }

class LabChart : public QWidget
{
    Q_OBJECT

    QChart* chart;
    QVector< QLineSeries* > series;

    model::Component* component;

public:
    explicit LabChart(QWidget *parent = nullptr);
    ~LabChart();

    void setXLabel(const QString& label);
    void setYLabel(const QString& label);
    void setY2Label(const QString& label);
    void addAxis(QAbstractAxis* axis, Qt::Alignment alignment);
    void createChannel(const QString& name, bool visible = true, QAbstractAxis* xAxis = nullptr, QAbstractAxis* yAxis = nullptr);
    void createDefaultAxes();

    void setModel(model::Component* component);

    void setVisible(int channel, bool visible);

public slots:
    void setWaveform(model::Waveform* waveform);
    void setRange(model::Waveform* waveform);

private:
    Ui::LabChart *ui;

private slots:
    void appendSample(const struct model::Sample& sample);
};

} // namespace app
#endif // APP_LABCHART_H
