from machine import UART
import time

# Initialize UART
uart = UART(0, baudrate=9600, tx=0, rx=1)

# Initial PID values
Kp = 0.8
Ki = 0.3
Kd = 0.05

def auto_tune_pid(rpm, torque, temp):
    global Kp, Ki, Kd

    # Prototype threshold-based adaptation
    if temp > 60:
        Kp += 0.1
        Ki += 0.05
        Kd += 0.01

    elif temp < 40:
        Kp = max(0.5, Kp - 0.05)
        Ki = max(0.1, Ki - 0.02)
        Kd = max(0.01, Kd - 0.005)

    return Kp, Ki, Kd


while True:
    if uart.any():
        try:
            line = uart.readline().decode().strip()

            if line.startswith("RPM:"):
                parts = line.split(",")

                rpm = float(parts[0].split(":")[1])
                torque = float(parts[1].split(":")[1])
                temp = float(parts[2].split(":")[1])

                print(
                    f"Received -> RPM: {rpm}, "
                    f"Torque: {torque}, Temp: {temp}"
                )

                Kp, Ki, Kd = auto_tune_pid(rpm, torque, temp)

                out = f"PID:{Kp:.2f},{Ki:.2f},{Kd:.2f}\n"
                uart.write(out)

        except Exception as e:
            print("Error:", e)

    time.sleep(0.1)
