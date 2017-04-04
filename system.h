/*
  system.h - Заголовок для команд системного уровня и процессов в реальном времени
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
Определение битовой карты исполнителей системы. Используется внутренним протоколом реального времени в качестве флагов команды реального времени, 
который уведомляет основную программу о выполнении указанной команды реального времени асинхронно.
ПРИМЕЧАНИЕ. Исполнитель системы использует неназначенную 8-разрядную переменную volatile (предел 8 флагов.) Флаги по умолчанию 
всегда ложны, поэтому протоколу реального времени нужно только проверять наличие ненулевого значения, чтобы 
знать, когда выполняется команда в реальном времени .
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
Карта битов исполнителей аварийных сигналов.
ПРИМЕЧАНИЕ: EXEC_CRITICAL_EVENT - это необязательный флаг, который должен быть установлен с сигнальным флагом. Когда включено, 
это останавливает Grbl в бесконечный цикл, пока пользователь не узнает о проблеме и не выдает команду
 мягкого сброса. Например, для события с жестким лимитом требуется этот тип остановки и подтверждения.
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
#define STATE_IDLE          0      // Должен быть равен нулю. Нет флагов.
#define STATE_ALARM         bit(0) // В состоянии тревоги. Запирает все процессы g-кода. Доступ к настройкам.
#define STATE_CHECK_MODE    bit(1) // Режим проверки G-кода. Запирает только планировщик и движение.
#define STATE_HOMING        bit(2) // Выполнение цикла самонаведения
#define STATE_CYCLE         bit(3) // Выполняется цикл или выполняются движения.
#define STATE_HOLD          bit(4) // Удержание активного канала
#define STATE_SAFETY_DOOR   bit(5) // Дверь безопасности приоткрыта. Подача удерживает и отключает систему.
#define STATE_MOTION_CANCEL bit(6) // Отмена движения при удержании и возврате в режим ожидания.

// Определить состояния приостановки системы.
#define SUSPEND_DISABLE       0      // Должен быть равен нулю.
#define SUSPEND_ENABLE_HOLD   bit(0) // Включено. Указывает, что цикл активен и в настоящее время выполняется удержание.
#define SUSPEND_ENABLE_READY  bit(1) // Готов возобновить с помощью команды запуска цикла.
#define SUSPEND_ENERGIZE      bit(2) // Повторно активирует выход до возобновления.
#define SUSPEND_MOTION_CANCEL bit(3) // Отмена возобновления движения. Используется процедурой зондирования.


// Определение глобальных системных переменных
typedef struct {
  uint8_t abort;                 // Флаг прерывания системы. Силы возвращаются в основной цикл для сброса.
  uint8_t state;                 // Отслеживает текущее состояние Grbl.
  uint8_t suspend;               // Система приостанавливает переменную bitflag, которая управляет удерживаниями, отменяет и защищает дверь.
  uint8_t soft_limit;            // Отслеживает мягкие предельные ошибки для конечного автомата. (boolean)
  
  int32_t position[N_AXIS];      // Векторное положение машины в реальном времени (так называемый домашний) пошагово.
                                 // ПРИМЕЧАНИЕ: Это может потребоваться, чтобы быть изменчивой переменной, если возникают проблемы.                         

  int32_t probe_position[N_AXIS]; // Последнее положение датчика в координатах и шагах станка.
  uint8_t probe_succeeded;        // Треки, если последний цикл зондирования был успешным.
  uint8_t homing_axis_lock;       // Блокировка осей при введении лимитов. Используется в качестве маски движения оси в шаговом ISR.
} system_t;
extern system_t sys;

volatile uint8_t sys_probe_state;    // Значение состояния зондирования. Используется для координации цикла зондирования с помощью шагового ISR.
volatile uint8_t sys_rt_exec_state;  // Глобальная переменная bitflag исполнителя реального времени для управления состоянием. См. Битовые маски EXEC.
volatile uint8_t sys_rt_exec_alarm;  // Глобальная переменная bitflag исполнителя реального времени для установки различных аварийных сигналов.


// Инициализировать последовательный протокол
void system_init();

// Возвращает, если дверь безопасности открыта или закрыта, в зависимости от состояния штифта.
uint8_t system_check_safety_door_ajar();

// Выполняет внутреннюю системную команду, определенную как строка, начинающаяся с символа '$'
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
