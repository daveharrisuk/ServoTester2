# ServoTester2

Exercise servo movement while monitoring current demand from servo.

Servo movement sweeps either side of mid position 1.50ms duty cycle,
- sweep controlled by a potentiometer, from zero sweep through to max degree of sweep.
- 1000ms after a sweep, the sweep direction changes. 
A toggle switch inhibits the direction change (run or pause).

LEDs indicate servo current...
- Less than 500mA then Yellow light.
- Between 20mA and 500mA then Yellow and Green light.
- Greater than 500mA then Red light.
- Low VDD (<4.75v) will blink off Yellow at 500ms rate
