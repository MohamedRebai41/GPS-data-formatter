SIGNAL void push()  {
  int i;

  PORTC = 0x0;
  PORTC = (PORTC ^ 0x00002000);
  swatch(0.001);
	PORTC = 0x0;
}
