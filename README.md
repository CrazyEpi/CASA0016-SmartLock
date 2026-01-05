# Multi-Sensor Smart Door Lock System
This project was developed for the CASA0016: Making, Designing & Building Connected Sensor Systems. It is an integrated security system that reacts to its environment by combining fingerprint authentication and presence sensing.

## Concepts and Motivations
Problem: Traditional doors often fail to lock automatically upon closing, and retrieving keys is difficult when hands are occupied.
Solution: A system providing Fingerprint Entry for convenience and Automatic Exit via presence detection.

## Hardwares
Arduino MKR 1010 WiFi.
R307 Fingerprint Sensor.
DFRobot 24GHz mmWave Radar (9M range).
MG90s Servo (simulating lock/unlock movement).

## Functioning Logic
<img width="607" height="1012" alt="image" src="https://github.com/user-attachments/assets/bae4599c-9afa-40ad-9092-f2983e4a4a7f" />

This smart door lock system operates on a prioritized state-machine logic with the fingerprint sensor and the mmwave radar. Fingerprint recognition serves as the highest-priority, capable of triggering an immediate unlock or resetting the 10-second auto-lock countdown if the door is already open. Simultaneously, the mmWave radar employs a score-based algorithm to filter environmental noise or people passing by, the score will rise when movement is detected and drop when there is no movements. the system only triggers an unlock once this score hits the defined threshold of 13.

## Enclosure Iterations
First Enclosure: Basic functionality but lacked wire outlets and space.

Second Enclosure: Larger room but insufficient wire management space.

Third Enclosure (Final): Features a curvy aesthetic design, dedicated outlets for all sensors, and unified integration with the fingerprint sensor.

## Repository
There is no existing document about the fingerprint sensor, and I need to assign two pins as another pair of TX-RX for the finger print sensor. So I write this comCheck.ino program to check if the newly assigned TX-RX is working, and used a sample program to check if the fingerprint sensor is working. Other than this, the projectRadarAndServo.ino is the main program running in the arduino.
