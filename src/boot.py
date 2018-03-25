from gpiozero import LED
import time

boot = LED(23)
reset = LED(27)

boot.off()
time.sleep(0.1);
reset.off()
time.sleep(0.1);
reset.on()


