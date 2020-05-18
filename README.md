INTRO
================
This repository holds the code for a home automation system that I am creating from scratch using Arduino. This README will explain more details about this project, and how to get it running.
><i>This project is being developed on my free time, so development will be slow.</i>


<br>

#### Project Goal:
My goal is to create a cheaper (and possibly better) alternative to home automation systems on the market.

The system will consist of sensor hubs (or stations) that will be placed in each room of a house. Each hub will report wirelessly to the main hub which will compile all of the data and broadcast it on a secure server that the homeowner can access to view and control temperature, room activity, etc. 

<img src="https://i.imgur.com/FvHtHOC.jpg" alt="HAS board development"></img>
><i><b>Top:</b> Breadboarded HAS board <b>Bottom:</b> v1 of HAS PCB</i>


<br>

#### Components:

##### Sensor Hub:
Each sensor hub will consist of the following:
><i>The hub is still in development, so some of these items may change.</i>

<ul>
    <li>1x <a href="http://www.ebay.com/itm/221808097338?_trksid=p2057872.m2749.l2649&ssPageName=STRK%3AMEBIDX%3AIT">NRF24L01 Radio Transceiver</a></li>
    <li>1x <a href="https://www.pjrc.com/teensy/teensyLC.html">Teensy LC</a></li>
    <li>1x <a href="http://www.ebay.com/itm/271360547550?_trksid=p2057872.m2749.l2649&ssPageName=STRK%3AMEBIDX%3AIT">DHT11 Temperature and Humidity Sensor</a></li>
    <li>3x <a href="https://www.mouser.com/datasheet/2/348/br24t16-w-e-1807517.pdf">CBR24T16-WZ EEPROM</a></li>
    <li>2x 1k Resistors</li>
    <li>1x 330 Resistor</li>
    <li>2x 3mm OR 5mm Red LEDs</li>
</ul>

<br>

##### Server:
The server/main hub will likely consist of the following:

<ul>
    <li>Raspberry Pi Zero W running Arch</li>
    <li>More to come...</li>
</ul>

<br>

#### Setting Up:

##### Arch Linux:




<br>

##### Wiring Schematics:
The wiring schematics for the different versions.
<ul>
    <li><a href="http://www.schematics.com/project/home-automation-system-35296/">Rev1</a></li>
    <li><a href="https://www.schematics.com/project/has-rev2-168309/">Rev2</a></li>
</ul>

###### PCB Files v1
Eagle CAD board and schematic files for the different verions. 
<ul>
    <li><a href="https://drive.google.com/drive/folders/1upeD6Ly29SMLrQmRa750bNDWt7U-4B42?usp=sharing">Rev1</a></li>
    <li>Rev2 (no board created)</li>
</ul>

<br>

##### Radio Setup:
<ul>
    <li><a href="https://goo.gl/u8zBUV">NRF24L01 pinout</a></li>
</ul>

|NRF24L01  |Arduino |
|:--------:|:------:|
|1 (GND)  |GND      |
|2 (VCC)  |3.3V     |
|3 (CE)   |D9       |
|4 (CSN)  |D10      |
|5 (SCK)  |D13      |
|6 (MOSI) |D11      |
|7 (MISO) |D12      |
|8 (IRQ)  |n/a      |

<br>

###### Helpful References:
<ul>
    <li><a href="http://starter-kit.nettigo.eu/2014/connecting-and-programming-nrf24l01-with-arduino-and-other-boards/">NRF24L01 tutorial</a></li>
    <li><a href="http://tmrh20.github.io/RF24/classRF24.html">NRF24L01 Driver Class Documenation</a></li>
</ul>
<br>

---------
><a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc/4.0/88x31.png" /></a><br />This work by <span xmlns:cc="http://creativecommons.org/ns#" property="cc:attributionName">Demetrius Van Sickle</span> is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/">Creative Commons Attribution-NonCommercial 4.0 International License</a>.
