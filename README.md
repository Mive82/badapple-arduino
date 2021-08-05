# Bad apple Arduino port
Projekt iz kolegija Ugrađeni računalni sustavi.  
Ovaj projekt je port cijelog Bad apple videa na I2C OLED ekran spojen na Arduino ili Raspberry Pi.

## Korišten Hardver
- Arduino Nano
- SPI SD card adapter*
- 128x128 OLED sa SH1107 kontrolerom
- Raspberry Pi 1

## Softverski dijelovi
- Video enkoder
- Video dekoder
- SD* i OLED driveri

`* SD kartica potrebna samo za Arduino`
# Kako koristiti
**PREPORUKA:** Koristiti Linux (na Windowsima ništa nije testirano)  
**DISCLAIMER:** Nisam odgovoran za nikakav gubitak podataka, flashajte na vlastitu odgovornost


### Ovisnosti
- Programi za flashanje Arduina (`avr-gcc` i ostali)
- `GNU Make` za Makefile
- `Python 3`
- `pillow` library za python (`sudo pip install pillow`)

### Priprema videa
- odzipaj `tools/halfframes.zip`
- pokreni `./tools/enc.py ./tools/halfframes video.img`

## Arduino

- uzmi praznu SD karticu (**BITI ĆE OBRISANA**)
- flashaj `video.img` na SD karticu koristeći `dd` ili bilo koji program koji dopušta flashanje RAW podataka na uređaj
- spoji arduino na računalo
- prilagodi PORT u Makefile
- pokreni `make`
- stavi SD karticu u adapter

## Raspberry
- prebaci `rpi` folder na Raspberry Pi
- prebaci `video.img` u `rpi` folder na Raspberry
- na Raspberry-u pokreni  
```sh
g++ -O2 -s main.cpp -o main
./main video.img
```