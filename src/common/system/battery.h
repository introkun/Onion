#ifndef BATTERY_H__
#define BATTERY_H__

#include "utils/file.h"
#include "utils/process.h"
#include "system/system.h"

#define CHECK_BATTERY_TIMEOUT 15000 //ms

static int battery_check_time = 0;
static time_t battery_last_modified = 0;

/**
 * @brief Retrieve the current battery percentage as reported by batmon
 * 
 * @return int : Battery percentage (0-100) or 500 if charging
 */
int battery_getPercentage(void)
{
    FILE *fp;
    int percentage = -1;
    int retry = 3;

    while (percentage == -1 && retry > 0) {
        if (exists("/tmp/percBat")) {
            file_get(fp, "/tmp/percBat", "%d", &percentage);
            break;
        }
        else {
            printf_debug("/tmp/percBat not found (%d)\n", retry);

            if (!process_isRunning("batmon")) {
                printf_debug("bin/batmon not running (%d)\n", retry);
                break;
            }
        }

        retry--;
        msleep(100);
    }

    #ifndef PLATFORM_MIYOOMINI
    #ifdef LOG_DEBUG
    return 78;
    #endif
    #endif

    if (percentage == -1)
        percentage = 0; // show zero when percBat not found

    return percentage;
}

bool battery_isCharging(void)
{
    #ifdef PLATFORM_MIYOOMINI
    char charging = 0;
    int fd = open(GPIO_DIR2 "gpio59/value", O_RDONLY);

    if (fd < 0) {
        // export gpio59, direction: in
        file_write(GPIO_DIR1 "export", "59", 2);
        file_write(GPIO_DIR2 "gpio59/direction", "in", 2);
        fd = open(GPIO_DIR2 "gpio59/value", O_RDONLY);
    }

    if (fd >= 0) {
        read(fd, &charging, 1);
        close(fd);        
    }

    return charging == '1';
    #else
    return true;
    #endif
}

bool battery_hasChanged(int ticks, int *out_percentage)
{
    bool changed = false;
    if (ticks - battery_check_time > CHECK_BATTERY_TIMEOUT) {
        if (file_isModified("/tmp/percBat", &battery_last_modified)) {
            int current_percentage = battery_getPercentage();

            if (current_percentage != *out_percentage) {
                *out_percentage = current_percentage;
                changed = true;
            }
        }
        battery_check_time = ticks;
    }
    return changed;
}

#endif // BATTERY_H__
