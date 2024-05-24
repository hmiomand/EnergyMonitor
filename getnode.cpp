#include "getnode.h"
#include <QFile>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <QDebug>
#include <QProcess>
#include <QSysInfo>

const char* HWMON_ARM[] = {HWMON_ARM_mV, HWMON_ARM_mA, HWMON_ARM_uW};
const char* HWMON_MEM[] = {HWMON_MEM_mV, HWMON_MEM_mA, HWMON_MEM_uW};
const char* HWMON_G3D[] = {HWMON_G3D_mV, HWMON_G3D_mA, HWMON_G3D_uW};
const char* HWMON_KFC[] = {HWMON_KFC_mV, HWMON_KFC_mA, HWMON_KFC_uW};

GetNode::GetNode(): usage()
{
    for (int i = 0; i < 8; i++) {
        cpu_node_list[i] = QString("/sys/devices/system/cpu/cpu%1/cpufreq/scaling_cur_freq").arg(i);
    }
}

void GetNode::GetSystemInfo()
{
    QProcess p1;
    QString cmd;

    cmd = "lsb_release -r | awk '{print $2}'";
    p1.start("bash", QStringList() << "-c" << cmd);
    p1.waitForFinished(-1);
    os_ver = p1.readAllStandardOutput().simplified();

    cmd = "lsb_release -i | awk '{print $3}'";
    p1.start("bash", QStringList() << "-c" << cmd);
    p1.waitForFinished(-1);
    os_name = p1.readAllStandardOutput().simplified();

    cmd = "uname -r";
    p1.start("bash", QStringList() << "-c" << cmd);
    p1.waitForFinished(-1);
    kernel_ver = p1.readAllStandardOutput().simplified();
}

QString GetNode::GetGPUCurFreq()
{
    // Measure seems to be invalid

    QFile *fp;
    QString freq;

    fp = new QFile(GPUFREQ_NODE);
    if (!fp->open(QIODevice::ReadOnly))
        return 0;
    freq = fp->readLine();
    freq = QString("%1").arg(freq.toInt() / 1000000);
    fp->close();
    delete fp;

    return freq;
}

QString GetNode::GetCPUCurFreq(int cpuNum)
{
    QFile *fp = new QFile(cpu_node_list[cpuNum]);
    QString freq;

    if (!fp->open(QIODevice::ReadOnly))
        return 0;

    freq = fp->readLine();
    freq = QString("%1").arg(freq.toInt()/1000);
    fp->close();

    delete fp;
    return freq;
}

QString GetNode::GetCPUTemp(int cpuNum)
{
    QFile *fp;

    char buf[16];

    fp = new QFile(TEMP_NODE + QString::number(cpuNum) + "/temp");

    if (!fp->open(QIODevice::ReadOnly)) {
        return 0;
    }

    fp->read(buf, 2);
    buf[2] = '\0';
    fp->close();
    delete fp;

    return buf;
}


int GetNode::open_sensor(const char *node, sensor_t *sensor, int unit)
{
    if ((sensor->fd[unit] = open(node, O_RDONLY)) < 0)
        qDebug() << node << "Open Fail";

    return sensor->fd[unit];
}

int GetNode::OpenINA231()
{
    for (int i = 0; i < NB_UNIT; i++) {
        if (open_sensor(HWMON_ARM[i], &sensor[SENSOR_ARM], i) < 0) {
            return -1;
        }
    }
    for (int i = 0; i < NB_UNIT; i++) {
        if (open_sensor(HWMON_MEM[i], &sensor[SENSOR_MEM], i) < 0) {
            return -1;
        }
    }
    for (int i = 0; i < NB_UNIT; i++) {
        if (open_sensor(HWMON_KFC[i], &sensor[SENSOR_KFC], i) < 0) {
            return -1;
        }
    }
    for (int i = 0; i < NB_UNIT; i++) {
        if (open_sensor(HWMON_G3D[i], &sensor[SENSOR_G3D], i) < 0) {
            return -1;
        }
    }

    return 0;
}

void GetNode::CloseINA231()
{
    for (int i = 0; i < NB_UNIT; i++) {
        close_sensor(&sensor[SENSOR_ARM], i);
    }
    for (int i = 0; i < NB_UNIT; i++) {
        close_sensor(&sensor[SENSOR_MEM], i);
    }
    for (int i = 0; i < NB_UNIT; i++) {
        close_sensor(&sensor[SENSOR_KFC], i);
    }
    for (int i = 0; i < NB_UNIT; i++) {
        close_sensor(&sensor[SENSOR_G3D], i);
    }
}

void GetNode::GetINA231()
{
    read_sensor(&sensor[SENSOR_ARM]);
    read_sensor(&sensor[SENSOR_MEM]);
    read_sensor(&sensor[SENSOR_KFC]);
    read_sensor(&sensor[SENSOR_G3D]);

    arm_mV = (float)(sensor[SENSOR_ARM].data.cur_mV / 100) / 10;
    arm_mA = (float)(sensor[SENSOR_ARM].data.cur_mA / 1) / 1000;
    arm_uW = (float)(sensor[SENSOR_ARM].data.cur_uW / 1000) / 1000;

    mem_mV = (float)(sensor[SENSOR_MEM].data.cur_mV / 100) / 10;
    mem_mA = (float)(sensor[SENSOR_MEM].data.cur_mA / 1) / 1000;
    mem_uW = (float)(sensor[SENSOR_MEM].data.cur_uW / 1000) / 1000;

    kfc_mV = (float)(sensor[SENSOR_KFC].data.cur_mV / 100) / 10;
    kfc_mA = (float)(sensor[SENSOR_KFC].data.cur_mA / 1) / 1000;
    kfc_uW = (float)(sensor[SENSOR_KFC].data.cur_uW / 1000) / 1000;

    g3d_mV = (float)(sensor[SENSOR_G3D].data.cur_mV / 100) / 10;
    g3d_mA = (float)(sensor[SENSOR_G3D].data.cur_mA / 1) / 1000;
    g3d_uW = (float)(sensor[SENSOR_G3D].data.cur_uW / 1000) / 1000;

}

void GetNode::read_sensor(sensor_t *sensor)
{
    // Read voltage in mV
    if (sensor->fd[VOLT] > 0) {

        char buffer[20];
        memset(buffer, 0, 20);

        if (pread(sensor->fd[VOLT], buffer, 20, 0) < 0) {
            qDebug() << "Sensor reading Error!";
        }
        sensor->data.cur_mV = atoi(buffer);
    }

    // Read current in mA
    if (sensor->fd[AMP] > 0) {

        char buffer[20];
        memset(buffer, 0, 20);

        if (pread(sensor->fd[AMP], buffer, 20, 0) < 0) {
            qDebug() << "Sensor reading Error!";
        }
        sensor->data.cur_mA = atoi(buffer);
    }

    // Read power in uW
    if (sensor->fd[WATT] > 0) {

        char buffer[20];
        memset(buffer, 0, 20);

        if (pread(sensor->fd[WATT], buffer, 20, 0) < 0) {
            qDebug() << "Sensor reading Error!";
        }
        sensor->data.cur_uW = atoi(buffer);
    }
}

void GetNode::close_sensor(sensor_t *sensor, int unit)
{
    if (sensor->fd[unit] > 0)
        close(sensor->fd[unit]);
}

int GetNode::calUsage(int cpu_idx, int user, int nice __attribute__((unused)), int system, int idle)
{
    long total = 0;
    long usage = 0;
    int diff_user, diff_system, diff_idle;

    diff_user = mOldUserCPU[cpu_idx] - user;
    diff_system = mOldSystemCPU[cpu_idx] - system;
    diff_idle = mOldIdleCPU[cpu_idx] - idle;

    total = diff_user + diff_system + diff_idle;
    if (total != 0)
        usage = diff_user * 100 / total;

    mOldUserCPU[cpu_idx] =user;
    mOldSystemCPU[cpu_idx] = system;
    mOldIdleCPU[cpu_idx] = idle;

    return usage;
}

int GetNode::GetCPUUsage(void)
{
    char buf[80] = {0,};
    char cpuid[] = "cpu";
    int findCPU = 0;
    int user, system, nice, idle;
    FILE *fp;
    int cpu_index = 0;

    fp = fopen("/proc/stat", "r");
    if (fp == NULL)
        return 0;

    while (fgets(buf, 80, fp)) {
        char temp[4] = "cpu";
        temp[3] = '0' + cpu_index;
        if (!strncmp(buf, temp, 4)) {
            findCPU = 1;
            sscanf(buf, "%s %d %d %d %d",
                   cpuid, &user, &nice, &system, &idle);
            usage[cpu_index] = calUsage(cpu_index, user, nice, system, idle);
            cpu_index++;
        }
        if (!strncmp(buf, "intr", 4))
            break;
        if (findCPU == 0)
            mOldUserCPU[cpu_index] = mOldSystemCPU[cpu_index] = mOldIdleCPU[cpu_index] = 0;
        else
            findCPU = 0;
    }

    fclose(fp);

    return 0;
}
