# Wearable-Controller
<img src="https://user-images.githubusercontent.com/110358483/185690280-f8a5eb79-bd52-4765-b8d7-2f6b82d8c509.png" width=70% height=70%>

## EXECUTIVE SUMMARY:

The objective of this project is to create a wearable device with the use of the NRF24L01 wireless RF Transceiver Module to control robots with multiple degrees of freedom. The most significant advantage is that it can interface with and manage any device that has an NRF24L01 sensor. The  Palm component is equipped with an MPU 6050 sensor that combines a 3-axis gyroscope and a 3-axis accelerometer. This enables the wearable device to track and transmit the actual hand's 3D trajectory. The finger joints are also equipped with angle sensors, which allows the device to increase the number of sensors at the joints to gain more control possibilities. 

## OPPORTUNITY:

Control robots with a variety of degrees of freedom.
Manage and interact with any NRF24L01 sensor-equipped device.
Equipped with an OLED to monitor current hand motion.

## CURRENT SITUATION:

<img src="https://user-images.githubusercontent.com/110358483/185690458-465b7d64-0f0e-4372-bf13-a515843a3179.jpg" width=50% height=50%>


This device is capable of controlling a robotic arm with 4 degrees of freedom (Check out my robot at https://github.com/jiayao09/RobotArm.git ) to grasp a target object. The video is as follows：




https://user-images.githubusercontent.com/110358483/204068545-e7c3f527-58eb-441f-b728-ce8197c82f1e.mov






A PDF file for detailed assembly is attached.
IMPORTANT: I have used a 3D printer (Ultimaker (FFF)) for all my parts, which has a tolerance of 0.3mm. Remember to change the hole size for a more precise printer.

<img src="https://user-images.githubusercontent.com/110358483/185690729-5c84a030-fac3-4585-99ff-56f6cac4a549.png" width=70% height=70%>

## ADDITIONAL DEVELOPMENT:

- Using the Kalman filter to get a more accurate location measurement.
- Increasing the number of sensors allows for more flexible control of robotic systems.
- Try this wearable device on our other robotic system.

## REFERENCE:

This model was developed based on Eben Kouao's model.  Check out his main page at https://www.patreon.com/ebenkouao. I change the sensor at the finger to have more degree of freedom. And also replace the Bluetooth transceiver with the NRF24L01 wireless RF transceiver to have a more stable data transition process.
