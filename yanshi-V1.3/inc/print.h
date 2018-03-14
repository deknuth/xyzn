#ifndef PRINT_H
#define PRINT_H
extern void _printf(const char *format,...);
extern void _gps_printf(const char *format,...);
extern void _hal_printf(const char *format,...);
extern void _mmw_printf(const char *format,...);
extern void _ker_printf(const char *format,...);
extern void _lid_printf(const char *format,...);

//#define HAL_DEBUG
//#define MMW_DEBUG
//#define GPS_DEBUG
//#define KERNEL_DEBUG
#define LIDAR_DEBUG
#endif // PRINT_H
