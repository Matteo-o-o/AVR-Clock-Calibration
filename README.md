# AVR-Clock-Calibration

## Development Environment

This repository includes a pre-configured **VS Code DevContainer** with the entire AVR GNU toolchain pre-installed (`avr-gcc`, `avr-libc`, `avrdude`, `make`). Opening this workspace in VS Code with the *Dev Containers* extension gives you an instant, zero-setup build environment.




Note perso temporaire : 

notre premiere fonction nous donne le nombre de tick réel pour faire 1/f_ref secondes 

cherche le meilleur OSCCAL (celui qui donnera 8000 tick dans notre cas) (recherche dicotomie)