# Quick Draw! 

## Josiah Medina & Gavin Hageman

## Design Summary

The project's design is simple and straightforward, using an Adafruit 1.8" TFT LCD screen (Figure 1) to display needed images for the game. 
Along with the TFT LCD screen, there are 2 buttons connected to the Arduino to track inputs. (Figure 2)

## Figure 1. AdaFruit TFT LCD Screen w/ Display
![image](https://github.com/SkinnyJosiah/Capstone_QuickDraw/blob/main/Images/IMG_1937.jpg)

## Figure 2. Buttons w/ Arduino

![image](https://github.com/SkinnyJosiah/Capstone_QuickDraw/blob/main/Images/qddiagram.png)

## System Details (Gavin-- Screenshots of the model and how it can hold the ARDUINO)

## Design Evaluation

Element Categories are listed like so...

### Output Display

Simple, we use a display screen for this. This is our AdaFruit 1.8" TFT LCD Screen/Shield.

### Manual User Input

This category consists of 2 main inputs- the joystick on the AdaFruit shield and the 2 buttons wired to the Arduino for gameplay.

### No use needed of Automatic Sensors or Actuators, Mechanisms & Hardware

### Logic, Processing, and Control; AND Miscellaneous

The project is built almost entirely off of programmed logic to decide how the game is run, for example, who has X amount of lives, who loses, who wins. What menu can go to menu X, so on and so forth. The game is based off of approximately 500 lines of complex rendering code and many functions for handling events.

Menu-Driven software is potentially the icon of this project. When you load the Arduino project you can browse the menu to access the game using the joystick on the AdaFruit Shield. 

Data storage and retrievel is vital for history of buttons or gameplay to determine turns, lives, wins and losses.

### Parts List

1. AdaFruit 1.8" TFT LCD Shield (v1)

2. Arduino Uno 

3. 2 Mini-Breadboards

4. 2 Buttons

5. Power Supply to Arduino

# Lessons Learned

### Oh where to start with this...

Lesson 1 and ABSOLUTELY #1... Reaching out to other people is KEY!!! Josiah had the same error for about a whole month and after rendering the code several times, he decided to call his grandpa who works in computer-based fields like cybersecurity. After co-piloting another build of the code with only minor changes, the error that was there for so long, preventing progress as a whole, was now gone! (gavin put entry here)

Lesson 2, 

Lesson 3, working with something new is hard! It takes time to pick up these things. For Josiah, it took almost a month just to get that AdaFruit Shield on the arduino correctly and using the correct libraries. For Gavin, 

### Instructions on how to build the QuickDraw project.

(make a video)

Take apart project, show shield and breadboards seperately, with LEDS as a disclaimer that you need extra power for these

Simply put on shield, connect buttons / LEDS to pins. Turn on and done.

# TO DO

Detailed wiring diagrams. (work on this, its supa ez)

CAD files for any custom parts and assemblies in stl format. (gavin)

Well-commented code in a file (or multiple files) which are named appropriately with the proper file extensions.  These could be of different file types (i.e. you create a CRONTAB file for your Raspberry Pi which starts a Python script). (done)

Your demonstration video will also be added to the repo to be preserved forever.

![image](https://github.com/SkinnyJosiah/Capstone_QuickDraw/blob/main/Images/title.png)

### [Original Inspiration](https://www.youtube.com/watch?v=m0CMZ2H_uWk&pp=ygUVMSwyIHN3aXRjaCBxdWljayBkcmF3)

### Beat your enemies to the queue, but don't jump the gun!

### **Quick Draw is a game where you follow queues to fire. The game will have different sound queues to throw users off, such as any other word that starts with F.**

**Here's a run-down of the rules.**
1. Two players start the game with their hands above the buttons. Randomly, different sound queues will play.
2. Upon each sound queue, a time-reliant function will activate. During this margin of time, the function will track if the user hits the button.
3. If the button(s) is/are hit during this time, the first button that was hit will be tracked.
4. If the sound queue, "FIRE" was played, then the player that hit the button wins that round.
5. Players will have a total of 3 lives to "bring suspense" into the game. Once a player drains the opponent's 3 lives, they win the game.

**What if a player presses a button on an incorrect sound queue other than, "Fire" ?**
- That player will lose their next turn, unable to press their button on the next sound queue.

### Miscellanious Project Resources

[View Initial Project Proposal](https://github.com/SkinnyJosiah/Capstone_QuickDraw/blob/main/Images/Initial_Proposal.pdf)

[View Status Check 1 - February 29](https://github.com/SkinnyJosiah/Capstone_QuickDraw/blob/main/Status%20Checks/Status_Check_1.md)

[View Status Check 2 - March 27](https://github.com/SkinnyJosiah/Capstone_QuickDraw/blob/main/Status%20Checks/Status_Check_2.md)

### Project Helpful Links and Such

[View Workarounds (Useful for blocked resource that may be needed)](https://github.com/SkinnyJosiah/Capstone_QuickDraw/blob/main/Workarounds.md)
