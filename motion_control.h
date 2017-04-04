//  motion_control.h - high level interface for issuing motion commands

#ifndef motion_control_h
#define motion_control_h


#define HOMING_CYCLE_LINE_NUMBER -1

// Execute linear motion in absolute millimeter coordinates. Feed rate given in millimeters/second
// unless invert_feed_rate is true. Then the feed_rate means that the motion should be completed in
// (1 minute)/feed_rate time.
#ifdef USE_LINE_NUMBERS
void mc_line(float *target, float feed_rate, uint8_t invert_feed_rate, int32_t line_number);
#else
void mc_line(float *target, float feed_rate, uint8_t invert_feed_rate);
#endif

// Execute an arc in offset mode format. position == current xyz, target == target xyz, 
// offset == offset from current xyz, axis_XXX defines circle plane in tool space, axis_linear is
// the direction of helical travel, radius == circle radius, is_clockwise_arc boolean. Used
// for vector transformation direction.
#ifdef USE_LINE_NUMBERS
void mc_arc(float *position, float *target, float *offset, float radius, float feed_rate, 
  uint8_t invert_feed_rate, uint8_t axis_0, uint8_t axis_1, uint8_t axis_linear, uint8_t is_clockwise_arc, int32_t line_number);
#else
void mc_arc(float *position, float *target, float *offset, float radius, float feed_rate,
  uint8_t invert_feed_rate, uint8_t axis_0, uint8_t axis_1, uint8_t axis_linear, uint8_t is_clockwise_arc);
#endif
  
// Остановить на определенное количество секунд
void mc_dwell(float seconds);

// Выполните цикл поиска, чтобы найти машинный нуль. Требуются концевые выключатели.
void mc_homing_cycle();

// Выполните цикл измерения длины инструмента. Требуется датчик.
#ifdef USE_LINE_NUMBERS
void mc_probe_cycle(float *target, float feed_rate, uint8_t invert_feed_rate, uint8_t is_probe_away,
  uint8_t is_no_error, int32_t line_number);
#else
void mc_probe_cycle(float *target, float feed_rate, uint8_t invert_feed_rate, uint8_t is_probe_away,
  uint8_t is_no_error);
#endif

// Выполняет системный сброс. Если в состоянии движения, убивает все движение и устанавливает системную тревогу.
void mc_reset();

#endif
