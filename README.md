# VibroBot

Software and hardware for VibroBot
(For the manuscript "VibroBot: A Lightweight and Wirelessly Programmable Vibration Bot for Haptic Guidance" submitted to IEEE Robotics and Automation Letters.)

Hardware: sch and pcb fiels of MCU board and Power board for VibroBot. 

Softeare: 
(1) datasets: run each python script in this folder firstly to generate the basic vibration pattern series (need COMSOL 6.0).
(2) algorithm_SA: the whole simulated annealing process for search the optimal phase spectrum (need the server memory no less than 100G).
(3) algorithm_SA_target_4: set any target feedback point to get its corresponding optimal phase spectrum (can change the target point in datasets/pattern_target_4).
