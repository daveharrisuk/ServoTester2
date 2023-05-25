# ServoTester2

Exercise servo movement while monitoring current demand from servo.

Servo movement sweeps either side of mid position 1.50ms duty cycle,
- sweep controlled by a potentiometer, from zero sweep through to max degree of sweep.
- 1000ms after a sweep, the sweep direction changes. 
A toggle switch inhibits the direction change (run or pause).
 While in pause, the servo will track any pot movements.

3 LEDs indicate servo current... (currents are average NOT peak)
- Less than 250mA then Yellow light.
- Between 25mA and 250mA then Yellow and Green light.
- Greater than 250mA then Red light.
- Low VDD (<4.70v) will blink off Yellow at 500ms rate (operation suspended for 2sec).

Revision A - required a 3D printed panel. Rsense was 0R5.

Revision B1 - the PCB is the panel. 
 Rsense reduced to 0R2. Con: reduced minimum detection current. Pro: lower Voltage loss on servo.
