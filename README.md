**NeuroNav is a custom quadcopter flight controller built on the ESP32 platform, designed for educational, research, and embedded systems development.
The project focuses on low-level flight control implementation, including sensor fusion, PID stabilization, receiver decoding, and ESC motor control.

This project demonstrates how a full drone flight controller can be implemented from scratch using ESP32, without relying on traditional STM32 flight control boards.**


Project Overview

The goal of this project is to design and implement a fully functional quadcopter flight controller using:

ESP32 microcontroller
MPU6050 IMU sensor
FlySky RF transmitter and receiver
Electronic Speed Controllers (ESC)
Brushless DC motors

The controller reads sensor data from the IMU, processes orientation using filtering algorithms, and stabilizes the drone through PID control loops.

Key Features

Custom ESP32 flight controller
MPU6050 IMU integration
Complementary filter for attitude estimation
Dual-loop PID stabilization system
Manual RF control using FlySky transmitter
ESC control using PWM signals
Custom designed PCB flight controller
Motor mixing for quadcopter X configuration
Real-time debugging through serial monitor

References used for this project: Joop Brokking : https://www.youtube.com/@Joop_Brokking
                                  Carbon Aeronautics : https://www.youtube.com/@carbonaeronautics
