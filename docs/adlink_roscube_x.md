ADLINK ROScube-I ARM
============

* ADLINK ROScube-I ARM

  ROS 2-enabled robotic controller based on NVIDIA® Jetson AGX Xavier™ module

  For more detail, you can refer to the following link:

  https://www.adlinktech.com/Products/ROS2_Solution/ROS2_Controller/ROScube-X


## Pin Mapping 


| Linux GPIO (/sys/class/gpio) | Function  | MRAA number | MRAA number | Function  | Linux GPIO (/sys/class/gpio) |
| :--------------------------: | :-------: | :---------: | :---------: | :-------: | :--------------------------: |
|                              | Not Used  |      1      |      2      | Not Used  |                              |
|                              | Not Used  |      3      |      4      | Not Used  |                              |
|                              | Not Used  |      5      |      6      | Not Used  |                              |
|             127              |   GPIO0   |      7      |      8      |   GPIO1   |             128              |
|             129              |   GPIO2   |      9      |     10      |   GPIO3   |             130              |
|             131              |   GPIO4   |     11      |     12      |   GPIO5   |             132              |
|             133              |   GPIO6   |     13      |     14      |   GPIO7   |             134              |
|             135              |   GPIO8   |     15      |     16      |   GPIO9   |             136              |
|             137              |   GPIO10  |     17      |     18      |   GPIO11  |             138              |
|             139              |   GPIO12  |     19      |     20      |   GPIO13  |             140              |
|             141              |   GPIO14  |     21      |     22      |   GPIO15  |             142              |
|             119              |   GPIO16  |     23      |     24      |   GPIO17  |             120              |
|             121              |   GPIO18  |     25      |     26      |   GPIO19  |             123              |
|                              |   CAN_TX  |     27      |     28      | SPI0_SCK  |                              |
|                              |   CAN_RX  |     29      |     30      | SPI0_MISO |                              |
|                              |   CANH    |     31      |     32      | SPI0_MISO |                              |
|                              |   CANL    |     33      |     34      | SPI0_CS   |                              |
|                              |   PWM     |     35      |     36      | I2C0_SDA  |                              |
|                              |   5V DC   |     37      |     38      | I2C0_SCL  |                              |
|                              |   5V DC   |     39      |     40      | 3.3V DC   |                              |
|                              |   GND     |     41      |     42      |    GND    |                              |
|                              |   GND     |     43      |     44      |    GND    |                              |

