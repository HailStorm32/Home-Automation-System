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
    <li>1x <a href="http://www.ebay.com/itm/181943830888?_trksid=p2057872.m2749.l2649&ssPageName=STRK%3AMEBIDX%3AIT">Ardunio Nano</a></li>
    <li>1x <a href="http://www.ebay.com/itm/271360547550?_trksid=p2057872.m2749.l2649&ssPageName=STRK%3AMEBIDX%3AIT">DHT11 Temperature and Humidity Sensor</a></li>
    <li>3x <a href="http://www.mouser.com/ds/2/405/cd4072b-407430.pdf">CD4072B Dual 4-Input OR Gates</a></li>
    <li>7x 1k Resistors</li>
    <li>1x <a href="http://www.ebay.com/itm/301122784490?_trksid=p2057872.m2749.l2649&ssPageName=STRK%3AMEBIDX%3AIT">8 position DIP Switch</a></li>
    <li>1x 3mm OR 5mm Red LED</li>
</ul>

<br>

##### Server:
The server/main hub will likely consist of the following:
><i>The server/main hub has not been developed, so all of these items are just ideas and may change.</i>
><i>The server/main hub will not be started until the sensor hub has reached about 75% completion.</i>


<ul>
    <li>Raspberry Pi running Arch</li>
    <li>More to come...</li>
</ul>

<br>

#### Setting Up:

##### The IDE:
To program the sensor hubs I am using Microsoft Visual Studio Community 2015 with an extension called Visual Micro that enables the ability to deploy to Arduino boards.
><i>Even though this code can be written in the Arduino IDE, the large size of the code and numerous tools that Visual Studio offers makes it a lot easier to use VS.</i>

<ol>
    <li><a href="https://go.microsoft.com/fwlink/?LinkId=691978&clcid=0x409">Download VS Community 2015</a></li>
    <li>Download/Install <a href="http://www.visualmicro.com/page/Arduino-Visual-Studio-Downloads.aspx">Visual Micro extension</a></li>
    <li>Download and install the Arduino IDE <i>(Visual Micro uses some of its libraries)</i></li>
    <li>Put the folder(s) that are in the "Libraries" folder into Arduino's Libraries folder.</li>
</ol>

<br>

##### Wiring Schematic:
The wiring schematic for this project can be found <a href="http://www.schematics.com/project/home-automation-system-35296/">here</a>.

###### PCB Files v1
Eagle CAD board and schematic files can be found <a href="https://drive.google.com/drive/folders/1upeD6Ly29SMLrQmRa750bNDWt7U-4B42?usp=sharing">here</a>.

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
