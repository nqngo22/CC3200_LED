# CC3200_LED

This program polls the SW2 and SW3 switches on the CC3200 Launchpad in order to implement a Moore state machine. In order to get from the reset state to the goal state S3, the user must press and hold SW2, then press SW3 without releasing SW2 and then release SW2 while continuing to press SW3. In the state S3, an LED in series with a current limiting resistor of about 330 ohms and connecting to pin 18 and either +3.3V or ground will flash at a rate of 5 Hz.
