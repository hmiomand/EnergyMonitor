#ifndef GETNODE_H
#define GETNODE_H

#include <QWidget>
#include <QFile>
#include <string>
#include <sys/ioctl.h>

#define GPUFREQ_NODE    "/sys/bus/platform/drivers/mali/11800000.gpu/devfreq/11800000.gpu/cur_freq"
#define TEMP_NODE       "/sys/class/thermal/thermal_zone"

typedef struct ina231_iocreg__t {
    char name[20];
    unsigned int enable;
    unsigned int cur_mV;
    unsigned int cur_mA;
    unsigned int cur_uW;
} ina231_iocreg_t;

typedef struct sensor__t {
    int  fd[3];
    ina231_iocreg_t data;
} sensor_t;

#define HWMON_ARM_mV    "/sys/class/hwmon/hwmon0/in1_input"
#define HWMON_ARM_mA    "/sys/class/hwmon/hwmon0/curr1_input"
#define HWMON_ARM_uW    "/sys/class/hwmon/hwmon0/power1_input"

#define HWMON_MEM_mV    "/sys/class/hwmon/hwmon1/in1_input"
#define HWMON_MEM_mA    "/sys/class/hwmon/hwmon1/curr1_input"
#define HWMON_MEM_uW    "/sys/class/hwmon/hwmon1/power1_input"

#define HWMON_G3D_mV    "/sys/class/hwmon/hwmon2/in1_input"
#define HWMON_G3D_mA    "/sys/class/hwmon/hwmon2/curr1_input"
#define HWMON_G3D_uW    "/sys/class/hwmon/hwmon2/power1_input"

#define HWMON_KFC_mV    "/sys/class/hwmon/hwmon3/in1_input"
#define HWMON_KFC_mA    "/sys/class/hwmon/hwmon3/curr1_input"
#define HWMON_KFC_uW    "/sys/class/hwmon/hwmon3/power1_input"

#define NB_CORE 8

enum {
    SENSOR_ARM = 0,
    SENSOR_MEM,
    SENSOR_KFC,
    SENSOR_G3D,
    SENSOR_MAX
};

enum {
    VOLT = 0,
    AMP,
    WATT,
    NB_UNIT
};

class GetNode
{
public:
    GetNode();
    QString cpu_node_list[NB_CORE];
    float arm_mV, arm_mA, arm_uW;
    float g3d_mV, g3d_mA, g3d_uW;
    float kfc_mV, kfc_mA, kfc_uW;
    float mem_mV, mem_mA, mem_uW;
    int usage[NB_CORE];
    QString os_name1;
    QString os_name;
    QString os_ver;
    QString kernel_ver;

    QString GetGPUCurFreq(void);
    QString GetCPUCurFreq(int cpuNum);
    QString GetCPUTemp(int cpuNum);
    int GetCPUUsage(void);
    int calUsage(int cpu_idx, int user, int nice, int system, int idle);

    int open_sensor(const char *node, sensor_t *sensor, int unit);
    void close_sensor(sensor_t *sensor, int unit);
    void read_sensor(sensor_t *sensor);

    int OpenINA231(void);
    void CloseINA231(void);
    void GetINA231(void);
    void GetSystemInfo(void);

private:
    sensor_t sensor[SENSOR_MAX];
    int mOldUserCPU[NB_CORE];
    int mOldSystemCPU[NB_CORE];
    int mOldIdleCPU[NB_CORE];

};

#endif // GETNODE_H
