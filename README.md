# SD Card Interfacing

Library was introduced as tool for interfacing AVR microcontrollers with memory card **sd**. These implementation has wide range of uses. The possibilities are much wider with the FAT32 file system implementation. The example provided in the main file uses the FAT32 file system implementation.

## Library

C library is aimed for communicating with SD Card through 4-wire SPI interface.

### Versions

1.0.0 - the first publication, init sequence and read block of data.

## Dependencies

### Usage

Prior defined for MCU Atmega328p / Atmega8.

| PORT | Arduino UNO R3 | Atmega8 / Atmega328 |      Device      |
| :--: | :------------: | :-----------------: | :--------------: |
| SCK  |       13       |         PB5         |  SD Card Clock   |
| MISO |       12       |         PB4         | SD Card DATA OUT |
| MOSI |       11       |         PB3         | SD Card DATA IN  |
|  CS  |       10       |         PB2         |  SD Card CS/SS   |
| SDA  |       A4       |         PC4         |     OLED SDA     |
| SCL  |       A5       |         PC5         |     OLED SCL     |

### Tested

Library was tested and proved with **Data Logger Arduino Shield** and **Arduino UNO R3**. This hardware configuration was chosen for simplicity. Arduino does not have bootloader and was connected through SPI. Communication with SD Card was displayed on 0.96″ 128x64 OLED Display (SSD1306 driver).

## Init Sequence

```c
/*
+----------------------------------------+
|             INIT SD CARD               |
+----------------------------------------+
                    |
                    v
+----------------------------------------+
|                SPI INIT                |
|----------------------------------------|
|        SPI_MASTER | SPI_MODE_0         |
|     SPI_MSB_FIRST | SPI_FOSC_DIV_16    |
+----------------------------------------+
                    |
                    v
+----------------------------------------+
|               POWER UP                 |
|----------------------------------------|
|       wait 250 ms powerUp time         |
|     deactivate CS - hold CS high       |
|    wait 1 ms - supply ramp up time     |
|  send 10 dummy bytes (min 74 cycles)   |
|     deactivate CS / hold CS high       | accor. [rjhcoding]
|            send dummy byte             | accor. [rjhcoding]
+----------------------------------------+
                    |
                    v
+----------------------------------------+
|         CMD0 / GO_IDLE_STATE           |
------------------------------------------
|   Is the software reset command and    |
|     sets each card into Idle State     |
|   regardlessof the current card state  |
+----------------------------------------+
                    |
                    v
+----------------------------------------+ CMD8 also enables to expand new
|          CMD8 / SEND_IF_COND           | functionality to some existing commands
|----------------------------------------| by redefining previously reserved bits
|     voltage information and asks       |
|    the accessed card whether card      |----------------------+
|  can operate in suppliedvoltage range  |                      |
+----------------------------------------+                      |
                    | YES                                       | NO
                    v                                           v
+----------------------------------------+  +----------------------------------------+
|           CARD RETURN RESPONSE         |  |              NO RESPONSE               |
------------------------------------------  ------------------------------------------
|      Accepted voltage range 2,7-3,6V   |  |       Ver2.00 or later SD Card         |
|      Ver2.00 or later SD Memory Card   |  |  (voltage mismatch) or Ver1.X SD Card  |
|                                        |  |         or not SD Memory Card          |
+----------------------------------------+  +----------------------------------------+
                    |                                           |
                    v                                           v
+----------------------------------------+  +----------------------------------------+
|        CMD41 / SD_SEND_OP_COND         |  |       CMD41 / SD_SEND_OP_COND /        |
|            with HCS = 0 or 1           |  |             with HCS = 0               |
------------------------------------------  ------------------------------------------
|          Activates the card’s          |  |         Activates the card’s           |
|         initialization process         |  |        initialization process          |
+----------------------------------------+  +----------------------------------------+
                   |                                            |
                   v                                            v
         +-------------------+ NO                     +-------------------+ NO
         |  Card is ready ?  |-------------------+    |  Card is ready ?  |----+
         +-------------------+                   |    +-------------------+    |
                   | YES                         |              | YES          |
                   v                             |              |              |
         +-------------------+ NO, CCS=0         |              |              |
         |  CCS in response? |----+              |              |              |
         +-------------------+    |              |              |              |
                   | YES, CCS=1   |              |              |              |
                   v              v              v              v              v
             +-----------+  +-----------+  +-----------+  +-----------+  +-----------+
             | Vers2.00+ |  | Vers2.00+ |  | UNUSABLE  |  |  Vers1.X  |  | UNUSABLE  |
             | SDHC,SDXC |  | SDSC Card |  |   CARD    |  | SDSC Card |  |   CARD    |
             +-----------+  +-----------+  +-----------+  +-----------+  +-----------+
*/
```

## Demonstration

# Acknowledgement

- [ryanj1234](https://github.com/ryanj1234) / [rjcoding](http://www.rjhcoding.com/avrc-sd-interface-1.php)
- [1nv1](https://github.com/1nv1/ulibSD/tree/master)

# Links

- [Physical Layer Simplified Specification](https://www.sdcard.org/downloads/pls/)
- [SD/SDHC Card Interfacing with ATmega8 /32](https://www.dharmanitech.com/2009/01/sd-card-interfacing-with-atmega8-fat32.html)
- [Generování zvuků pomocí mikrokontrolérů, David Matoušek](https://www.preskoly.sk/p/370712-generovani-zvuku-pomoci-mikrokontroleru/)
