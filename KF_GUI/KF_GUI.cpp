#include "KF_GUI.h"

#include <iostream>

KF_GUI::KF_GUI(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
}



void KF_GUI::on_browseButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), "C://", "All files (*.*)");

    ui.inputFileLineEdit->setText(filename);

    inputFile = filename;
}

typedef int(_fastcall* MyProc1)();

void KF_GUI::on_executeButton_clicked()
{
    ui.statusLebel->setText(QString("Running"));
    ui.statusLebel->setStyleSheet("QLabel { color: rgb(0, 122, 204);; }");

    HINSTANCE hLib = NULL;

    if ((hLib = LoadLibrary(L"KF_Cpp.dll")) == NULL)
    {

    }

    MyProc1 hello = (MyProc1)GetProcAddress(hLib, "filter");

    ui.statusLebel->setText(QString::number(hello()));

    FreeLibrary(hLib);
    


    //ui.statusLebel->setText(QString("Done"));
    //ui.statusLebel->setStyleSheet("QLabel { color: rgb(0, 255, 0); }");
}



void KF_GUI::on_plotButton_clicked()
{

}



void KF_GUI::on_threadNumberSlider_valueChanged()
{
    usedThreads = ui.threadNumberSlider->value();

    ui.usedThreadsLebel->setText(QString::number(usedThreads));
}