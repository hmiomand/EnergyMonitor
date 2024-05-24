#include "displaysysinfo.h"
#include "ui_displaysysinfo.h"
#include "getnode.h"
#include <QTimer>
#include <QDebug>
#include <QPalette>
#include <QLabel>

DisplaySysInfo::DisplaySysInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DisplaySysInfo)
{
    ui->setupUi(this);

    armPlotData.index = 0;
    memPlotData.index = 0;
    kfcPlotData.index = 0;
    g3dPlotData.index = 0;

    a15Volt = a15Ampere = a15Watt = "";
    a7Volt = a7Ampere = a7Watt = "";
    gpuVolt = gpuAmpere = gpuWatt = "";
    memVolt = memAmpere = memWatt = "";

    getNode = new GetNode();

    SENSOR_OPEN = 1;

    if (getNode->OpenINA231()) {
        qDebug() << "OpenINA231 error";
        SENSOR_OPEN = 0;
        QLabel *lb = new QLabel(this);
        lb->setText("no Energy Sensors");
        lb->setGeometry(420, 230,130, 50);
    } else {
        ARMSensorCurve = new QwtPlotCurve();
        MEMSensorCurve = new QwtPlotCurve();
        KFCSensorCurve = new QwtPlotCurve();
        G3DSensorCurve = new QwtPlotCurve();
        displaySensorPlot();
    }

    getNode->GetSystemInfo();

    ui->losname->setText(getNode->os_name + " : " + getNode->os_ver);
    ui->lkernel->setText("Kernel : " + getNode->kernel_ver);
    ui->lsw_ver->setText("Energy Monitor 1.3");

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);

}

DisplaySysInfo::~DisplaySysInfo()
{
    getNode->CloseINA231();
    delete ui;
}

void DisplaySysInfo::displaySensorPlot()
{
    ui->qwtPlotSensor->setAxisScale(QwtPlot::yLeft, 0, 5);
    ui->qwtPlotSensor->setAxisScale(QwtPlot::xBottom, 0, 100);
    ui->qwtPlotSensor->setAxisTitle(QwtPlot::xBottom, "sec");
    ui->qwtPlotSensor->setAxisTitle(QwtPlot::yLeft, "Watt");
    ui->qwtPlotSensor->setCanvasBackground(QBrush(QColor(0, 0, 0)));
    ui->qwtPlotSensor->setTitle("Watt Graph");

    ARMSensorCurve->attach(ui->qwtPlotSensor);
    ARMSensorCurve->setPen(QColor(50, 160, 140));

    MEMSensorCurve->attach(ui->qwtPlotSensor);
    MEMSensorCurve->setPen(QColor(255, 0, 0));

    KFCSensorCurve->attach(ui->qwtPlotSensor);
    KFCSensorCurve->setPen(QColor(0, 255, 0));

    G3DSensorCurve->attach(ui->qwtPlotSensor);
    G3DSensorCurve->setPen(QColor(200, 160, 50));
}

void DisplaySysInfo::drawARMSensorCurve()
{
    if (getNode->arm_uW > 0 && getNode->arm_uW < 10) {
        if (armPlotData.index < NB_POINT_PLOT - 1) {
            armPlotData.yData[armPlotData.index] = getNode->arm_uW;
            armPlotData.xData[armPlotData.index] = armPlotData.index;
            armPlotData.index++;
        } else {
            armPlotData.yData[NB_POINT_PLOT - 1] = getNode->arm_uW;
            for (int i = 0; i < NB_POINT_PLOT - 1; i++) {
                armPlotData.yData[i] = armPlotData.yData[i + 1];
            }
        }
    }

    ARMSensorCurve->setSamples(armPlotData.xData, armPlotData.yData, armPlotData.index);
    ui->qwtPlotSensor->replot();
}

void DisplaySysInfo::drawMEMSensorCurve()
{
    if (getNode->mem_uW > 0 && getNode->mem_uW < 10) {
        if (memPlotData.index < NB_POINT_PLOT - 1) {
            memPlotData.yData[memPlotData.index] = getNode->mem_uW;
            memPlotData.xData[memPlotData.index] = memPlotData.index;
            memPlotData.index++;
        } else {
            memPlotData.yData[NB_POINT_PLOT - 1] = getNode->mem_uW;
            for (int i = 0; i < NB_POINT_PLOT - 1; i++) {
                memPlotData.yData[i] = memPlotData.yData[i + 1];
            }
        }
    }

    MEMSensorCurve->setSamples(memPlotData.xData, memPlotData.yData, memPlotData.index);
    ui->qwtPlotSensor->replot();
}

void DisplaySysInfo::drawKFCSensorCurve()
{
    if (getNode->kfc_uW > 0 && getNode->kfc_uW < 10) {
        if (kfcPlotData.index < NB_POINT_PLOT - 1) {
            kfcPlotData.yData[kfcPlotData.index] = getNode->kfc_uW;
            kfcPlotData.xData[kfcPlotData.index] = kfcPlotData.index;
            kfcPlotData.index++;
        } else {
            kfcPlotData.yData[NB_POINT_PLOT - 1] = getNode->kfc_uW;
            for (int i = 0; i < NB_POINT_PLOT - 1; i++) {
                kfcPlotData.yData[i] = kfcPlotData.yData[i + 1];
            }
        }
    }

    KFCSensorCurve->setSamples(kfcPlotData.xData, kfcPlotData.yData, kfcPlotData.index);
    ui->qwtPlotSensor->replot();
}

void DisplaySysInfo::drawG3DSensorCurve()
{
    if (getNode->g3d_uW > 0 && getNode->g3d_uW < 10) {
        if (g3dPlotData.index < NB_POINT_PLOT - 1) {
            g3dPlotData.yData[g3dPlotData.index] = getNode->g3d_uW;
            g3dPlotData.xData[g3dPlotData.index] = g3dPlotData.index;
            g3dPlotData.index++;
        } else {
            g3dPlotData.yData[NB_POINT_PLOT - 1] = getNode->g3d_uW;
            for (int i = 0; i < NB_POINT_PLOT - 1; i++) {
                g3dPlotData.yData[i] = g3dPlotData.yData[i + 1];
            }
        }
    }

    G3DSensorCurve->setSamples(g3dPlotData.xData, g3dPlotData.yData, g3dPlotData.index);
    ui->qwtPlotSensor->replot();
}

void DisplaySysInfo::DisplaySensor()
{
    getNode->GetINA231();

    float2string();

    ui->ARMmVlcd->setPalette(QColor(100, 100, 100));
    ui->ARMmAlcd->setPalette(QColor(100, 100, 100));
    ui->ARMuWlcd->setPalette(QColor(100, 100, 100));
    ui->ARMmVlcd->display(a15Volt);
    ui->ARMmAlcd->display(a15Ampere);
    ui->ARMuWlcd->display(a15Watt);

    ui->KFCmVlcd->setPalette(QColor(100, 100, 100));
    ui->KFCmAlcd->setPalette(QColor(100, 100, 100));
    ui->KFCuWlcd->setPalette(QColor(100, 100, 100));
    ui->KFCmVlcd->display(a7Volt);
    ui->KFCmAlcd->display(a7Ampere);
    ui->KFCuWlcd->display(a7Watt);

    ui->G3DmVlcd->setPalette(QColor(100, 100, 100));
    ui->G3DmAlcd->setPalette(QColor(100, 100, 100));
    ui->G3DuWlcd->setPalette(QColor(100, 100, 100));
    ui->G3DmVlcd->display(gpuVolt);
    ui->G3DmAlcd->display(gpuAmpere);
    ui->G3DuWlcd->display(gpuWatt);

    ui->MEMmVlcd->setPalette(QColor(100, 100, 100));
    ui->MEMmAlcd->setPalette(QColor(100, 100, 100));
    ui->MEMuWlcd->setPalette(QColor(100, 100, 100));
    ui->MEMmVlcd->display(memVolt);
    ui->MEMmAlcd->display(memAmpere);
    ui->MEMuWlcd->display(memWatt);
}

void DisplaySysInfo::float2string()
{
    a15Volt = QString("%1").arg(getNode->arm_mV, 0, 'f', 3);
    a15Ampere = QString("%1").arg(getNode->arm_mA, 0, 'f', 3);
    a15Watt = QString("%1").arg(getNode->arm_uW, 0, 'f', 3);

    a7Volt = QString("%1").arg(getNode->kfc_mV, 0, 'f', 3);
    a7Ampere = QString("%1").arg(getNode->kfc_mA, 0, 'f', 3);
    a7Watt = QString("%1").arg(getNode->kfc_uW, 0, 'f', 3);

    gpuVolt = QString("%1").arg(getNode->g3d_mV, 0, 'f', 3);
    gpuAmpere = QString("%1").arg(getNode->g3d_mA, 0, 'f', 3);
    gpuWatt = QString("%1").arg(getNode->g3d_uW, 0, 'f', 3);

    memVolt = QString("%1").arg(getNode->mem_mV, 0, 'f', 3);
    memAmpere = QString("%1").arg(getNode->mem_mA, 0, 'f', 3);
    memWatt = QString("%1").arg(getNode->mem_uW, 0, 'f', 3);
}

void DisplaySysInfo::displayCpuFrequency()
{
    ui->GPUFreqEdit->setText(getNode->GetGPUCurFreq());
    ui->CPU0FreqEdit->setText(getNode->GetCPUCurFreq(0));
    ui->CPU1FreqEdit->setText(getNode->GetCPUCurFreq(1));
    ui->CPU2FreqEdit->setText(getNode->GetCPUCurFreq(2));
    ui->CPU3FreqEdit->setText(getNode->GetCPUCurFreq(3));
    ui->CPU4FreqEdit->setText(getNode->GetCPUCurFreq(4));
    ui->CPU5FreqEdit->setText(getNode->GetCPUCurFreq(5));
    ui->CPU6FreqEdit->setText(getNode->GetCPUCurFreq(6));
    ui->CPU7FreqEdit->setText(getNode->GetCPUCurFreq(7));
    ui->CPU4TempEdit->setText(getNode->GetCPUTemp(0));
    ui->CPU5TempEdit->setText(getNode->GetCPUTemp(1));
    ui->CPU6TempEdit->setText(getNode->GetCPUTemp(2));
    ui->CPU7TempEdit->setText(getNode->GetCPUTemp(3));
    ui->GPUTempEdit->setText(getNode->GetCPUTemp(4));
}

void DisplaySysInfo::update()
{
    displayCpuFrequency();
    if (SENSOR_OPEN) {
        DisplaySensor();
        drawARMSensorCurve();
        drawMEMSensorCurve();
        drawKFCSensorCurve();
        drawG3DSensorCurve();
    }
//    getNode->GetCPUUsage();
//    ui->CPU0UsageEdit->setText(QString::number(getNode->usage[0]));
//    ui->CPU1UsageEdit->setText(QString::number(getNode->usage[1]));
//    ui->CPU2UsageEdit->setText(QString::number(getNode->usage[2]));
//    ui->CPU3UsageEdit->setText(QString::number(getNode->usage[3]));
//    ui->CPU4UsageEdit->setText(QString::number(getNode->usage[4]));
//    ui->CPU5UsageEdit->setText(QString::number(getNode->usage[5]));
//    ui->CPU6UsageEdit->setText(QString::number(getNode->usage[6]));
//    ui->CPU7UsageEdit->setText(QString::number(getNode->usage[7]));
}
