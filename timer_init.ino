
// TCCR1A - Timer/Counter1 Control Register A
// Initial Value 0x00
// • Bit 7:6 – COM1A1:0: Compare Output Mode for Channel A (R/W)
// • Bit 5:4 – COM1B1:0: Compare Output Mode for Channel B (R/W)
// • Bits 3, 2 (R)
// • Bit 1:0 – WGM11:0: Waveform Generation Mode (R/W)

// TCCR1B - Timer/Counter1 Control Register B
// Initial Value 0x00
// • Bit 7 – ICNC1: Input Capture Noise Canceler (R/W)
// • Bit 6 – ICES1: Input Capture Edge Select (R/W)
// • Bit 5 – Reserved Bit (R)
// • Bit 4:3 – WGM13:2: Waveform Generation Mode (R/W)
// • Bit 2:0 – CS12:0: Clock Select (R/W)

// ICR1 - Input Capture Register (16 bit)
// TCNT1 - Timer Counter (16 bit)
// OCR1A - Output Comparison Register (16 bit)
// OCR1A - Output Comparison Register (16 bit)

// TIMSK1 – Timer/Counter1 Interrupt Mask Register
// Initial Value 0x00
// • Bit 7, 6 – Res: Reserved Bits (R)
// • Bit 5 – ICIE1: Timer/Counter1, Input Capture Interrupt Enable
// • Bit 4, 3 – Res: Reserved Bits (R)
// • Bit 2 – OCIE1B: Timer/Counter1, Output Compare B Match Interrupt Enable
// • Bit 1 – OCIE1A: Timer/Counter1, Output Compare A Match Interrupt Enable
// • Bit 0 – TOIE1: Timer/Counter1, Overflow Interrupt Enable

// DDRB – The Port B Data Direction Register

void timerInitialisation( void ) {
  // Stop the timer while we muck with it
  TCCR1B =
      (0 << ICNC1) | (0 << ICES1) |
      (0 << WGM13) | (0 << WGM12) |
      (0 << CS12) | (0 << CS11) | (0 << CS10);
 
  // Set the timer to mode 14...
  // Mode  WGM13  WGM12  WGM11  WGM10  Timer/Counter Mode of Operation  TOP   Update of OCR1x at TOV1  Flag Set on
  //              CTC1   PWM11  PWM10
  // ----  -----  -----  -----  -----  -------------------------------  ----  -----------------------  -----------
  // 14    1      1      1      0      Fast PWM                         ICR1  BOTTOM                   TOP
 
  // Set output on Channel A to...
  // COM1A1  COM1A0  Description
  // ------  ------  -----------------------------------------------------------
  // 1       0       Clear OC1A/OC1B on Compare Match (Set output to low level).
  TCCR1A =
      (1 << COM1A1) | (0 << COM1A0) |   // COM1A1, COM1A0 = 1, 0
      (0 << COM1B1) | (0 << COM1B0) |
      (1 << WGM11) | (0 << WGM10);      // WGM11, WGM10 = 1, 0
 
  // Set TOP to...
  // fclk_I/O = 16000000
  // N        = 1
  // TOP      = 1999
  // fOCnxPWM = fclk_I/O / (N * (1 + TOP))
  // fOCnxPWM = 16000000 / (1 * (1 + 1999))
  // fOCnxPWM = 16000000 / 2000
  // fOCnxPWM = 8000
  ICR1 = 1999; // Ensure the first slope is complete
  TCNT1 = 0; // Ensure Channel B is irrelevant
  OCR1B = 0; // Ensure Channel A starts at zero / off
  OCR1A = 0; // We don't need no stinkin interrupts
 
  TIMSK1 = (0 << ICIE1) | (0 << OCIE1B) | (0 << OCIE1A) | (0 << TOIE1); // Ensure the Channel A pin is configured for output
  DDRB |= (1 << DDB1);

  // Start the timer...
  // CS12  CS11  CS10  Description
  // ----  ----  ----  ------------------------
  // 0     0     1     clkI/O/1 (No prescaling)
  TCCR1B =
      (0 << ICNC1) | (0 << ICES1) |
      (1 << WGM13) | (1 << WGM12) |              // WGM13, WGM12 = 1, 1
      (0 << CS12) | (0 << CS11) | (1 << CS10);
}
