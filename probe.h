//  probe.h - ���, ����������� � ������� ������������
  
#ifndef probe_h
#define probe_h 

//��������, ������������ ��������� ��������� ��������.
#define PROBE_OFF     0 // ������������ ��������� ��� �� ������������. (������ ���� ����� ����.)
#define PROBE_ACTIVE  1 //�������� ���������� �� ������� ���������.

// ��������� ������������� ��������� �����.
void probe_init();
/*
// Called by probe_init() and the mc_probe() routines. Sets up the probe pin invert mask to 
// appropriately set the pin logic according to setting for normal-high/normal-low operation 
// and the probing cycle modes for toward-workpiece/away-from-workpiece. 
*/
// ���������� ����������� probe_init () � mc_probe ().
// ������������� ������������� ����� ���� �������, ����� ��������������� ������� ���������� ������ �����
// � ������������ � ���������� ��� ������ � ����������-������� / ����������-������ ������� �
// �������� ����� ������������ �� ����������� � ���������/����� �� ���������..
void probe_configure_invert_mask(uint8_t is_probe_away);

// Returns probe pin state. Triggered = true. Called by gcode parser and probe state monitor.
// ���������� ��������� ����. Triggered = true. ���������� ������������ gcode � ��������� ��������� �����.
uint8_t probe_get_state();

// Monitors probe pin state and records the system position when detected. Called by the
// stepper ISR per ISR tick.
// ������������ ��������� ���� ������� � ��������� ��������� ������� ��� �����������. ���������� ������� ISR �� ���� ISR.
void probe_state_monitor();

#endif
