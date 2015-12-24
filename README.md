INTRO
================
This repository holds the code for a home automation system that I am creating from scratch using Arduino. This README will explain more details about this project, and how to get it running.
><i>This project will take second priority over the <a href="https://github.com/HailStorm32/Quadcopter-Code">Autonomous Quad Project</a>, and so development will be slow.</i>

<br>
####Project Goal:
The system will consist of sensor hub (or stations) that will placed in each room of a house. Each hub will report wirelessly to the main hub which will compile all of the data and broadcast it on a secure server that the home owner can access to view room activity, temperature, etc. 

<br>
####Components:
<br>
#####Sensor Hub:
Each sensor hub will consist of the following:
><i>The hub is still in development, so some of these items are just ideas and may change.</i>


<ul>
    <li>Arduino Nano</li>
    <li><a href="http://www.ebay.com/itm/10-Pcs-NRF24L01-2-4GHz-RF-Antenna-Wireless-Transceiver-Module-For-Arduino-/221808097338?hash=item33a4caf83a:g:83gAAMXQgwNSKaUc">NRF24L01 + 2.4GHz RF Antenna Wireless Transceiver Module</a></li>
    <li>Temperature Sensor</li>
    <li>Motion Sensor (i.e. Sonar)</li>
</ul>
<br>
#####Server:
The server/main hub will likely consist of the following:
><i>The server/main hub has not been developed, so all of these items are just ideas and may change.</i>
><i>The server/main hub will not be started on until the sensor hub has reach about 75% completion.</i>


<ul>
    <li><a href="http://www.newegg.com/Product/Product.aspx?Item=N82E16813455003">Humming Board i2eX</a> running Ubuntu</li>
    <li>More to come...</li>
</ul>

<br>
####Setting Up:
Just put the folder(s) that are in the "Libraries" folder into Arduino's Libraries folder.

<br>
#####Radio Setup:
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
#####Helpful References:
<ul>
    <li><a href="http://starter-kit.nettigo.eu/2014/connecting-and-programming-nrf24l01-with-arduino-and-other-boards/">NRF24L01 tutorial</a></li>
    <li><a href="http://tmrh20.github.io/RF24/classRF24.html">NRF24L01 Driver Class Documenation</a></li>
</ul>
<br>

---------
><a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc/4.0/88x31.png" /></a><br />This work by <span xmlns:cc="http://creativecommons.org/ns#" property="cc:attributionName">Demetrius Van Sickle</span> is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/">Creative Commons Attribution-NonCommercial 4.0 International License</a>.
