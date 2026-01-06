# Multi-Sensor Smart Door Lock System

<img width="547" height="619" alt="QQ_1767668857129" src="https://github.com/user-attachments/assets/35979bfc-0972-4490-8442-05c785f8f667" />

This project was developed for the CASA0016: Making, Designing & Building Connected Sensor Systems. It is an integrated security system that reacts to its environment by combining fingerprint authentication and presence sensing.

## Concepts and Motivations

<img width="444" height="456" alt="QQ_1767667886307" src="https://github.com/user-attachments/assets/d7e1e840-59a8-4fdd-bda2-b3f9266ad34f" />

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
<img width="879" height="560" alt="29ac236d14d5126d7ae3381369c9a971" src="https://github.com/user-attachments/assets/c341be4e-6d5e-43b5-9ca9-e08537503053" />

First Enclosure: Basic functionality but lacked wire outlets and space.

<img width="879" height="560" alt="fce1b5eb-5191-4d94-a1a2-8da7f5c14518" src="https://github.com/user-attachments/assets/b9ad46db-54a1-4fb8-a18d-40342c5f8c8b" />

Second Enclosure: Larger room but insufficient wire management space.

<img width="501" height="491" alt="c341e5a3-d255-4371-b0bd-cc4c2f18f6f4" src="https://github.com/user-attachments/assets/810ededb-c856-4205-9091-5135ba528a3b" />

Third Enclosure (Final): Features a curvy aesthetic design, dedicated outlets for all sensors, and unified integration with the fingerprint sensor.

## Repository

comCheck.ino: Check if the TX-RX pin is working.

projectRadarAndServo.ino: Main program for the arduino, handling all logics

There is no existing document about the fingerprint sensor, and I need to assign two pins as another pair of TX-RX for the finger print sensor. So I write this comCheck.ino program to check if the newly assigned TX-RX is working, and used a sample program to check if the fingerprint sensor is working. Other than this, the projectRadarAndServo.ino is the main program running in the arduino.
