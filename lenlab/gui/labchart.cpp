#include "labchart.h"
#include "ui_labchart.h"

#include <QtCharts>
#include <QtPrintSupport/QtPrintSupport>

namespace gui {

LabChart::LabChart(QWidget *parent) :
    QWidget(parent),
    m_chart(new QChart),
    ui(new Ui::LabChart)
{
    ui->setupUi(this);

    m_chart->legend()->hide();
    ui->chartView->setChart(m_chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);

    ui->label_y2->hide();
}

LabChart::~LabChart()
{
    delete ui;
}

void
LabChart::setLabelX(QString text)
{
    ui->label_x->setText(text);
}

void
LabChart::setLabelY(QString text)
{
    ui->label_y->setText(text);
}

void
LabChart::setLabelY2(QString text)
{
    ui->label_y2->setText(text);
    ui->label_y2->show();
}

void
LabChart::addSeries(QLineSeries *series)
{
    m_chart->addSeries(series);
    m_curves.append(series);
}

QList< QPointer< QLineSeries > >
LabChart::series() const
{
    return m_curves;
}

void
LabChart::addAxis(QAbstractAxis *axis, Qt::Alignment alignment)
{
    m_chart->addAxis(axis, alignment);
}

void
LabChart::createDefaultAxes()
{
    m_chart->createDefaultAxes();
}

void
LabChart::replace(model::pSeries const & series)
{
    QList< QPointF > points(series->getLength());

    for (unsigned int channel = 0; channel < m_curves.size(); ++channel) {
        for (unsigned int i = 0; i < series->getLength(); ++i) {
            points[i] = QPointF(series->getX(i), series->getY(i, channel));
        }

        m_curves.at(channel)->replace(points);
    }

    setRange(series);
}

void
LabChart::appendLast(model::pSeries const & series)
{
    for (unsigned int channel = 0; channel < m_curves.size(); ++channel) {
        m_curves.at(channel)->append(series->getLastX(), series->getLastY(channel));
    }

    setRange(series);
}

void
LabChart::setRange(model::pSeries const & series)
{
    auto axes = m_chart->axes(Qt::Horizontal);
    if (!axes.isEmpty()) axes.first()->setRange(series->getMinX(), series->getMaxX());

    axes = m_chart->axes(Qt::Vertical);
    if (!axes.isEmpty()) axes.first()->setRange(series->getMinY(0), series->getMaxY(0));
    if (axes.size() > 1) axes.at(1)->setRange(series->getMinY(1), series->getMaxY(1));
}

void
LabChart::setChannelVisible(unsigned int channel, bool visible)
{
    Q_ASSERT(channel < m_curves.size());

    m_curves.at(channel)->setVisible(visible);
}

void
LabChart::print(QString filename)
{
    QPrinter printer = QPrinter(QPrinter::HighResolution);
    printer.setOutputFileName(filename);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize::A6);
    printer.setPageOrientation(QPageLayout::Landscape);

    QPrintDialog dialog = QPrintDialog(&printer);
    dialog.setWindowTitle("Print Plots");
    if (dialog.exec() != QDialog::Accepted)
        return;

    QPainter painter;
    painter.begin(&printer);

    auto pageRect = printer.pageRect(QPrinter::Point);
    resize(pageRect.width(), pageRect.height());

    qreal scale = printer.resolution() / 72.0;
    painter.scale(scale, scale);

    render(&painter);

    painter.end();
}

} // namespace gui
