/*
  probe.c - Код, относящийся к методам зондирования
  Part of Grbl

*/
  
#include "grbl.h"


// Инвертирует состояние пина датчика в зависимости от настроек пользователя и режима цикла зондирования.
uint8_t probe_invert_mask;


// Процедура инициализации контактов зонда.
void probe_init() 
{
  PROBE_DDR &= ~(PROBE_MASK); //Настройка в качестве входных контактов
  #ifdef DISABLE_PROBE_PIN_PULL_UP
    PROBE_PORT &= ~(PROBE_MASK); // Если в DDR 0 (вход), то в PORT выбирается тип входа:0 - HiZ(, 1 - PullUp)
  #else
    PROBE_PORT |= PROBE_MASK;    // Если в DDR 0 (вход), то в PORT выбирается тип входа: (0 - HiZ,) 1 - PullUp
  #endif
  // probe_configure_invert_mask(false); // Initialize invert mask. Not required. Updated when in-use.
}


// Called by probe_init() and the mc_probe() routines. Sets up the probe pin invert mask to 
// appropriately set the pin logic according to setting for normal-high/normal-low operation 
// and the probing cycle modes for toward-workpiece/away-from-workpiece. 
void probe_configure_invert_mask(uint8_t is_probe_away)
{
  probe_invert_mask = 0; // Инициализировать как ноль.
  if (bit_isfalse(settings.flags,BITFLAG_INVERT_PROBE_PIN)) { probe_invert_mask ^= PROBE_MASK; }
  if (is_probe_away) { probe_invert_mask ^= PROBE_MASK; }
}


// Returns the probe pin state. Triggered = true. Called by gcode parser and probe state monitor.
uint8_t probe_get_state() { return((PROBE_PIN & PROBE_MASK) ^ probe_invert_mask); }


// Monitors probe pin state and records the system position when detected. Called by the
// stepper ISR per ISR tick.
// NOTE: This function must be extremely efficient as to not bog down the stepper ISR.
void probe_state_monitor()
{
  if (sys_probe_state == PROBE_ACTIVE) {
    if (probe_get_state()) {
      sys_probe_state = PROBE_OFF;
      memcpy(sys.probe_position, sys.position, sizeof(sys.position));
      bit_true(sys_rt_exec_state, EXEC_MOTION_CANCEL);
    }
  }
}