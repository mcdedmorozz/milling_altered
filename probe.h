//  probe.h - Код, относящийся к методам зондирования
  
#ifndef probe_h
#define probe_h 

//Значения, определяющие состояние конечного автомата.
#define PROBE_OFF     0 // Тестирование отключено или не используется. (Должен быть равен нулю.)
#define PROBE_ACTIVE  1 //Активное наблюдение за входным контактом.

// Процедура инициализации контактов зонда.
void probe_init();
/*
// Called by probe_init() and the mc_probe() routines. Sets up the probe pin invert mask to 
// appropriately set the pin logic according to setting for normal-high/normal-low operation 
// and the probing cycle modes for toward-workpiece/away-from-workpiece. 
*/
// Вызывается процедурами probe_init () и mc_probe ().
// Устанавливает инвертирующую маску пина датчика, чтобы соответствующим образом установить логику пинов
// в соответствии с настройкой для работы в нормальном-высоком / нормальном-низком режимах и
// режимами цикла зондирования по направлению к заготовке/Вдали от заготовки..
void probe_configure_invert_mask(uint8_t is_probe_away);

// Returns probe pin state. Triggered = true. Called by gcode parser and probe state monitor.
// Возвращает состояние пина. Triggered = true. Вызывается анализатором gcode и монитором состояния зонда.
uint8_t probe_get_state();

// Monitors probe pin state and records the system position when detected. Called by the
// stepper ISR per ISR tick.
// Контролирует состояние пина датчика и фиксирует положение системы при обнаружении. Вызывается шаговым ISR по типу ISR.
void probe_state_monitor();

#endif
