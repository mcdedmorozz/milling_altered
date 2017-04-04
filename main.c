/*
  main.c - An embedded CNC Controller with rs274/ngc (g-code) support
  Встроенный контроллер CNC с поддержкой rs274 / ngc (g-code)
  Part of Grbl
*/

#include "grbl.h"


// Declare system global variable structure
// Объявление системной глобальной структуры переменных
system_t sys; 


int main(void)
{
  // Инициализация системы при включении питания.
  serial_init();   // Установка скорости и прерываний в последовательном режиме
  settings_init(); // Загрузить настройки Grbl из EEPROM
  stepper_init();  // Настройка шаговых контактов и таймеров прерывания
  system_init();   // Конфигурируйте контакты выводов и прерывание смены контактов
  
  memset(&sys, 0, sizeof(system_t));  // Очистить все системные переменные
  sys.abort = true;   // Завершить прерывание для завершения инициализации
  sei(); // Разрешить прерывания

  // Check for power-up and set system alarm if homing is enabled to force homing cycle
  // by setting Grbl's alarm state. Alarm locks out all g-code commands, including the
  // startup scripts, but allows access to settings and internal commands. Only a homing
  // cycle '$H' or kill alarm locks '$X' will disable the alarm.
  // NOTE: The startup script will run after successful completion of the homing cycle, but
  // not after disabling the alarm locks. Prevents motion startup blocks from crashing into
  // things uncontrollably. Very bad.
  #ifdef HOMING_INIT_LOCK
    if (bit_istrue(settings.flags,BITFLAG_HOMING_ENABLE)) { sys.state = STATE_ALARM; }
  #endif
  
  // Force Grbl into an ALARM state upon a power-cycle or hard reset.
  #ifdef FORCE_INITIALIZATION_ALARM
    sys.state = STATE_ALARM;
  #endif
  
  // Grbl initialization loop upon power-up or a system abort. For the latter, all processes
  // will return to this loop to be cleanly re-initialized.
  for(;;) {

    // TODO: Отдельная задача настройки, требующая отключения прерываний, 
	// особенно прерывание работы системы, и чистка всех активных прерываний.
  
    // Сбросить первичные системы Grbl.
    serial_reset_read_buffer(); // Очистить последовательный буфер чтения
    gc_init(); // Установить парсер g-кода в состояние по умолчанию
    spindle_init();
    coolant_init();
    limits_init(); 
    probe_init();
    plan_reset(); // Очистить буфер блока и переменные планировщика
    st_reset(); // Очистить переменные подсистемы.

    // Синхронизация очистила координаты g-кода и планировщика до текущего положения системы.
    plan_sync_position();
    gc_sync_position();

    // Сброс системных переменных.
    sys.abort = false;
    sys_rt_exec_state = 0;
    sys_rt_exec_alarm = 0;
    sys.suspend = false;
    sys.soft_limit = false;
              
    // Запустите основной цикл Grbl. Входы процессов обрабатывают и выполняют их.
    protocol_main_loop();
    
  }
  return 0;   /* Не достигнуто */
}
