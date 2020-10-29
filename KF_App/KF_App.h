#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_KF_App.h"

class KF_App : public QMainWindow
{
    Q_OBJECT

public:
    KF_App(QWidget *parent = Q_NULLPTR);

private:
    Ui::KF_AppClass ui;
};
