/*
  system.h - ��������� ��� ������ ���������� ������ � ��������� � �������� �������
*/

#ifndef system_h
#define system_h

#include "grbl.h"

// Define system executor bit map. Used internally by realtime protocol as realtime command flags, 
// which notifies the main program to execute the specified realtime command asynchronously.
// NOTE: The system executor uses an unsigned 8-bit volatile variable (8 flag limit.) The default
// flags are always false, so the realtime protocol only needs to check for a non-zero value to 
// know when there is a realtime command to execute.
/*
����������� ������� ����� ������������ �������. ������������ ���������� ���������� ��������� ������� � �������� ������ ������� ��������� �������, 
������� ���������� �������� ��������� � ���������� ��������� ������� ��������� ������� ����������.
����������. ����������� ������� ���������� ������������� 8-��������� ���������� volatile (������ 8 ������.) ����� �� ��������� 
������ �����, ������� ��������� ��������� ������� ����� ������ ��������� ������� ���������� ��������, ����� 
�����, ����� ����������� ������� � �������� ������� .
*/
#define EXEC_STATUS_REPORT  bit(0) // bitmask 00000001
#define EXEC_CYCLE_START    bit(1) // bitmask 00000010
#define EXEC_CYCLE_STOP     bit(2) // bitmask 00000100
#define EXEC_FEED_HOLD      bit(3) // bitmask 00001000
#define EXEC_RESET          bit(4) // bitmask 00010000
#define EXEC_SAFETY_DOOR    bit(5) // bitmask 00100000
#define EXEC_MOTION_CANCEL  bit(6) // bitmask 01000000

// Alarm executor bit map.
// NOTE: EXEC_CRITICAL_EVENT is an optional flag that must be set with an alarm flag. When enabled,
// this halts Grbl into an infinite loop until the user aknowledges the problem and issues a soft-
// reset command. For example, a hard limit event needs this type of halt and aknowledgement.
/*
����� ����� ������������ ��������� ��������.
����������: EXEC_CRITICAL_EVENT - ��� �������������� ����, ������� ������ ���� ���������� � ���������� ������. ����� ��������, 
��� ������������� Grbl � ����������� ����, ���� ������������ �� ������ � �������� � �� ������ �������
 ������� ������. ��������, ��� ������� � ������� ������� ��������� ���� ��� ��������� � �������������.
*/
#define EXEC_CRITICAL_EVENT     bit(0) // bitmask 00000001 (SPECIAL FLAG. See NOTE:)
#define EXEC_ALARM_HARD_LIMIT   bit(1) // bitmask 00000010
#define EXEC_ALARM_SOFT_LIMIT   bit(2) // bitmask 00000100
#define EXEC_ALARM_ABORT_CYCLE  bit(3) // bitmask 00001000
#define EXEC_ALARM_PROBE_FAIL   bit(4) // bitmask 00010000
#define EXEC_ALARM_HOMING_FAIL  bit(5) // bitmask 00100000

// Define system state bit map. The state variable primarily tracks the individual functions
// of Grbl to manage each without overlapping. It is also used as a messaging flag for
// critical events.
#define STATE_IDLE          0      // ������ ���� ����� ����. ��� ������.
#define STATE_ALARM         bit(0) // � ��������� �������. �������� ��� �������� g-����. ������ � ����������.
#define STATE_CHECK_MODE    bit(1) // ����� �������� G-����. �������� ������ ����������� � ��������.
#define STATE_HOMING        bit(2) // ���������� ����� �������������
#define STATE_CYCLE         bit(3) // ����������� ���� ��� ����������� ��������.
#define STATE_HOLD          bit(4) // ��������� ��������� ������
#define STATE_SAFETY_DOOR   bit(5) // ����� ������������ ����������. ������ ���������� � ��������� �������.
#define STATE_MOTION_CANCEL bit(6) // ������ �������� ��� ��������� � �������� � ����� ��������.

// ���������� ��������� ������������ �������.
#define SUSPEND_DISABLE       0      // ������ ���� ����� ����.
#define SUSPEND_ENABLE_HOLD   bit(0) // ��������. ���������, ��� ���� ������� � � ��������� ����� ����������� ���������.
#define SUSPEND_ENABLE_READY  bit(1) // ����� ����������� � ������� ������� ������� �����.
#define SUSPEND_ENERGIZE      bit(2) // �������� ���������� ����� �� �������������.
#define SUSPEND_MOTION_CANCEL bit(3) // ������ ������������� ��������. ������������ ���������� ������������.


// ����������� ���������� ��������� ����������
typedef struct {
  uint8_t abort;                 // ���� ���������� �������. ���� ������������ � �������� ���� ��� ������.
  uint8_t state;                 // ����������� ������� ��������� Grbl.
  uint8_t suspend;               // ������� ���������������� ���������� bitflag, ������� ��������� �������������, �������� � �������� �����.
  uint8_t soft_limit;            // ����������� ������ ���������� ������ ��� ��������� ��������. (boolean)
  
  int32_t position[N_AXIS];      // ��������� ��������� ������ � �������� ������� (��� ���������� ��������) ��������.
                                 // ����������: ��� ����� �������������, ����� ���� ���������� ����������, ���� ��������� ��������.                         

  int32_t probe_position[N_AXIS]; // ��������� ��������� ������� � ����������� � ����� ������.
  uint8_t probe_succeeded;        // �����, ���� ��������� ���� ������������ ��� ��������.
  uint8_t homing_axis_lock;       // ���������� ���� ��� �������� �������. ������������ � �������� ����� �������� ��� � ������� ISR.
} system_t;
extern system_t sys;

volatile uint8_t sys_probe_state;    // �������� ��������� ������������. ������������ ��� ����������� ����� ������������ � ������� �������� ISR.
volatile uint8_t sys_rt_exec_state;  // ���������� ���������� bitflag ����������� ��������� ������� ��� ���������� ����������. ��. ������� ����� EXEC.
volatile uint8_t sys_rt_exec_alarm;  // ���������� ���������� bitflag ����������� ��������� ������� ��� ��������� ��������� ��������� ��������.


// ���������������� ���������������� ��������
void system_init();

// ����������, ���� ����� ������������ ������� ��� �������, � ����������� �� ��������� ������.
uint8_t system_check_safety_door_ajar();

// ��������� ���������� ��������� �������, ������������ ��� ������, ������������ � ������� '$'
uint8_t system_execute_line(char *line);

// Execute the startup script lines stored in EEPROM upon initialization
void system_execute_startup(char *line);

// Returns machine position of axis 'idx'. Must be sent a 'step' array.
float system_convert_axis_steps_to_mpos(int32_t *steps, uint8_t idx);

// Updates a machine 'position' array based on the 'step' array sent.
void system_convert_array_steps_to_mpos(float *position, int32_t *steps);

// CoreXY calculation only. Returns x or y-axis "steps" based on CoreXY motor steps.
#ifdef COREXY
  int32_t system_convert_corexy_to_x_axis_steps(int32_t *steps);
  int32_t system_convert_corexy_to_y_axis_steps(int32_t *steps);
#endif

#endif
