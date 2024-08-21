# VibroBot

Software and hardware materials for VibroBot  
(For the manuscript "VibroBot: A Lightweight and Wirelessly Programmable Vibration Bot for Haptic Guidance" submitted to IEEE Robotics and Automation Letters 2024.)

Hardware: sch and pcb fiels of MCU board and Power board for VibroBot. 

Software:   
(1) VibroBot_driver (C++): displays six vibration modes via LAN instructions.  
(2) Hand gesture capturing_Mediapipe (python): gets the 5*3 finger joint angles, and sends the angles to virtual scene via local port 12333.  
(3) Gesture display_Unity3D (C#): displays the finger joint status, computes the gesture errors, and sends errors via local port 12345.  
(4) Haptic guidance (python): gets gesture errors, determines the vibration modes for each VibroBot, and send modes via LAN.  

For more details, please refer to:   
https://github.com/chaixy1/VibroBot-Demo/tree/main
