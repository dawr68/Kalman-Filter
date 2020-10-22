#pragma once

#include <QtWidgets/QWidget>
#include "ui_KF_GUI.h"
#include <QFileDialog>
#include <QtCharts>

class KF_GUI : public QWidget
{
    Q_OBJECT

public:
    KF_GUI(QWidget *parent = Q_NULLPTR);

private slots:
    void on_browseButton_clicked();
    void on_executeButton_clicked();
    void on_plotButton_clicked();


    void on_threadNumberSlider_valueChanged();

private:
    Ui::KF_GUIClass ui;
    QString inputFile = "";
    int usedThreads = 1;
};
