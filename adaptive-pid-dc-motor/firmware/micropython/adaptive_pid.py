from machine import Pin, PWM
import time

# PID Class
class PID:
    def __init__(self, Kp, Ki, Kd, setpoint=0, output_limits=(0, 255)):
        self.Kp, self.Ki, self.Kd = Kp, Ki, Kd
        self.setpoint = setpoint
        self._min, self._max = output_limits
        self._last_error = 0
        self._integral = 0
        self._last_time = time.ticks_ms()

    def compute(self, measurement):
        now = time.ticks_ms()
        dt = time.ticks_diff(now, self._last_time) / 1000
        error = self.setpoint - measurement
        self._integral += error * dt

        derivative = 0
        if dt > 0:
            derivative = (error - self._last_error) / dt

        output = (
            self.Kp * error
            + self.Ki * self._integral
            + self.Kd * derivative
        )

        if output > self._max:
            output = self._max
        if output < self._min:
            output = self._min

        self._last_error = error
        self._last_time = now

        return output, abs(error)

    def set_tunings(self, Kp, Ki, Kd):
        self.Kp, self.Ki, self.Kd = Kp, Ki, Kd


# Encoder Setup
enc_pin = Pin(34, Pin.IN)
pulses = 0

def pulse_handler(pin):
    global pulses
    pulses += 1

enc_pin.irq(trigger=Pin.IRQ_RISING, handler=pulse_handler)

# Motor PWM Setup
motor_pwm = PWM(Pin(5), freq=1000, duty=0)

# Pulses per revolution
PPR = 20

def get_motor_rpm(interval_ms=500):
    global pulses

    start = pulses
    time.sleep_ms(interval_ms)
    end = pulses

    delta = end - start
    revs = delta / PPR
    rpm = (revs * 60000) / interval_ms

    return rpm

def set_motor_pwm(val):
    duty = int(val * 1023 / 255)
    motor_pwm.duty(duty)


# Main
pid = PID(
    Kp=1.5,
    Ki=0.5,
    Kd=0.05,
    setpoint=175,
    output_limits=(0, 255)
)

last_error = 0
last_tune = time.ticks_ms()

print("Starting self-tuning PID loop...")

while True:
    rpm = get_motor_rpm(200)
    output, error = pid.compute(rpm)
    set_motor_pwm(output)

    # Adaptive tuning every 500 ms
    if time.ticks_diff(time.ticks_ms(), last_tune) > 500:
        delta_error = error - last_error

        if error > 20:
            pid.Kp += 0.1
            pid.Ki += 0.01
            pid.Kd += 0.001

        elif delta_error < -10:
            pid.Kp *= 0.98
            pid.Ki *= 0.98
            pid.Kd *= 0.98

        # Constrain gains
        pid.Kp = min(max(pid.Kp, 0.5), 5.0)
        pid.Ki = min(max(pid.Ki, 0.1), 1.0)
        pid.Kd = min(max(pid.Kd, 0.01), 1.0)

        last_error = error
        last_tune = time.ticks_ms()

        print(
            "RPM:{:.1f} Out:{:.1f} Kp:{:.2f} Ki:{:.2f} Kd:{:.3f}"
            .format(rpm, output, pid.Kp, pid.Ki, pid.Kd)
        )
