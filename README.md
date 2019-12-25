#  ![JADEKU](img/watchman.gif) JADEKU - ADQ128 
## Portable Low-Power Data Acquisition Device 

### Description

This device is a low power microcontroller design, it has a serial data flash for data storage,
serial port communication interface, and an iButton 1-wire bus for data capture.
The main constrain was overall cost, ex. note the RTC implementation is done using microcontroller resources.
Basically we have a battery power, handheld device with storage up to 1Mb, RTC, 1-wire bus and serial connection 
for setup and data download.  The main application use case is a Watchman's clock; each station or check point for the routine
check has an iButton, DS1990 chip, it returns a uniquie electronic serial number,  the device stores it with the event's time.
When a person decides, ex. Supervisor/Manager, she can download the data stored to a PC, then she is able to do audits, 
check events, hours and station/point. 



### Design

Hardware block diagram:

![Block Diagram](img/jadeku-hw.jpg)

#### Embedded Software

![](autodoc/html/jadeku_8c_a6288eba0f8e8ad3ab1544ad731eb7667_cgraph.png)

[Doxygen documentation](https://hemonserrat.github.io/jadeku/)

| IDE:| MPLAB X IDE v5.25 |
| ------------------------------------------------------------ | ---- |
| Compiler:| XC8 v1.41 | 

See folders: <br>
[inc](inc/) <br>
[src](src/)

#### EDA: Schematics and PCB
![](img/icons-pdf-download.jpg) [EDA design PDF](hardware/0472-Jadeku2/JADEKU.pdf)

![Schematics](hardware/0472-Jadeku2/Schematics-jadeku.png)

![Schematics](img/ADQ0472-1.jpg)![Schematics](img/ADQ0472-3.jpg)

Source files inside [ECAD folder](hardware/0472-Jadeku2/) <br>
CAM [Gerbers folder](hardware/CAM)

#### CAD: Case V1

![CAD Case design](hardware/case/020-0472.png) <br>
[CAD files](hardware/case)

- - - 
- Commercial use case:  Watchman's clock  (routine control)

| ![KIT-JADEKU](img/KIT-JADEKU.gif)                          | Complete kit, including wall mount points, DS1990 iButtons, serial cable and PC Software      |
| ------------------------------------------------------------ | ---- |
| ![pic_jadeku](img/pic_jadeku.gif) | The device with its ruged metal case.|
| ![pic_nosotros](img/pic_nosotros.jpg) | Security guard/Watchman check in at an station point   |
| ![JADEKU-17](img/JADEKU-17.gif) | Handheld portable device  |
| ![JADEKU-2](img/JADEKU-2.gif) |  Dallas DS1990 1-wire check in station/point |




(C) 2004-2005,  Hernan Monserrat, License GNU GPL 3 <br>
![](img/OSHW-100.jpg)![](img/by-100.jpg) 


