#include "KF_GUI.h"

#include <iostream>

KF_GUI::KF_GUI(QWidget *parent)
    : QWidget(parent), filter(0)
{
    ui.setupUi(this);

    for (int i = 0; i < 6; i++) {
        chart[i] = 0;
        chartView[i] = 0;
    }
}


KF_GUI::~KF_GUI()
{
    for (auto &ch : chart)
        delete ch;

    for (auto &chv : chartView)
        delete chv;
}


void KF_GUI::setFilterManager(FilterManager* newFilter)
{
    filter = newFilter;
}


void KF_GUI::setupGraphs()
{
    for (auto &ser : series)
        ser = new QLineSeries;

    for (int i = 0, c = 0, r = 0, s = 0; i < 6; i++, c++, s+=2) {
        chart[i] = new QChart;
        chartView[i] = new QChartView(chart[i]);
        ui.chartsGrid->addWidget(chartView[i], r, c);
        if (c == 2) {
            c = -1;
            r++;
        }

        chart[i]->setMargins(QMargins(1, 1, 1, 1));
        chartView[i]->setRenderHint(QPainter::Antialiasing);
        chartView[i]->setRubberBand(QChartView::RectangleRubberBand);

        series[s]->setName(QString::fromStdString("Raw"));
        series[s + 1]->setName(QString::fromStdString("Filtered"));
        
        chart[i]->addSeries(series[s]);
        chart[i]->addSeries(series[s+1]);
        chart[i]->createDefaultAxes();

        chart[i]->legend()->detachFromChart();
        chart[i]->legend()->setGeometry(QRectF(300, 5, 200, 40));
    }

    chart[0]->setTitle("X Axis");
    chart[1]->setTitle("Y Axis");
    chart[2]->setTitle("Z Axis");
    chart[3]->setTitle("Acc X Axis");
    chart[4]->setTitle("Acc Y Axis");
    chart[5]->setTitle("Acc Z Axis");
}


void KF_GUI::on_browseButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), "C://", "All files (*.*)");

    ui.inputFileLineEdit->setText(filename);

    if (filter)
        filter->readRawDataFromFile(filename.toStdString());
}


void KF_GUI::on_executeButton_clicked()
{
    ui.statusLabel->setText(QString("Running"));
    ui.statusLabel->setStyleSheet("QLabel { color: rgb(0, 122, 204);; }");

    if (!filter)
        return;

    filter->execute();
    
    if (filter->getLastUsedLibrary() == 0)
        ui.asmTimeLabel->setText(QString::number(filter->getLastExeDur()));
    else
        ui.cppTimeLabel->setText(QString::number(filter->getLastExeDur()));

    ui.statusLabel->setText(QString("Done"));
    ui.statusLabel->setStyleSheet("QLabel { color: rgb(0, 255, 0); }");
}



void KF_GUI::on_plotButton_clicked()
{
    ui.statusLabel->setText(QString("Drawing"));
    ui.statusLabel->setStyleSheet("QLabel { color: rgb(0, 122, 204);; }");
    ui.statusLabel->update();

    float **rawDataArr = filter->getRawAngles();
    float **filteredDataArr = filter->getFilteredAngles();
    int count = filter->getDataSize();

    for (int i = 0, s=0; i < 3; i++, s+=2) {
        chart[i]->removeSeries(series[s]);
        chart[i]->removeSeries(series[s+1]);
        series[s]->clear();
        series[s+1]->clear();

        for (int j = 0; j < count; j++) {
            series[s]->append(j, rawDataArr[i][j]);
            series[s+1]->append(j, filteredDataArr[i][j]);
        }

        chart[i]->addSeries(series[s]);
        chart[i]->addSeries(series[s+1]);
        chart[i]->createDefaultAxes();
    }

    ui.statusLabel->setText(QString("Done"));
    ui.statusLabel->setStyleSheet("QLabel { color: rgb(0, 255, 0); }");
}


void KF_GUI::on_asmRB_clicked()
{
    filter->chooseAsmLibrary();
}


void KF_GUI::on_cppRB_clicked()
{
    filter->chooseCppLibrary();
}


void KF_GUI::on_threadNumberSlider_valueChanged()
{
    int threads_num = ui.threadNumberSlider->value();
    filter->setNumberOfThreads(threads_num);
    ui.usedThreadsNumber->setText(QString::number(threads_num));
}