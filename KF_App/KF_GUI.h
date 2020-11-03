#pragma once

#include <QtWidgets/QWidget>
#include "ui_KF_GUI.h"
#include <QFileDialog>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include "FilterManager.h"

using namespace QtCharts;

class KF_GUI : public QWidget
{
    Q_OBJECT

public:
    KF_GUI(QWidget *parent = Q_NULLPTR);
    ~KF_GUI();
    void setFilterManager(FilterManager* newFilter);
    void setupGraphs();

private slots:
    void on_browseButton_clicked();
    void on_executeButton_clicked();
    void on_plotButton_clicked();
    void on_asmRB_clicked();
    void on_cppRB_clicked();

    void on_threadNumberSlider_valueChanged();

private:
    Ui::KF_GUIClass ui;
    QChart *chart[6];
    QChartView *chartView[6];
    QLineSeries *series[12];
    QLineSeries *test1, test2;

    FilterManager *filter;
};
