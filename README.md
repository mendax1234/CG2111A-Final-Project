# CG2111A Final Project
## Alex Control Program
Once the hardware connections have been verified, upload the Arduino program called `Alex.ino` in `Alex` folder to the Arduino Uno. Other files`.ino` are tabs to the main file and should be compiled together. Ensure that they are recognised as tabs for the program to compile correctly.

### Compile
Next, compile the alex-pi.cpp program to control Alex:
```bash
gcc alex-pi.cpp serial.cpp serialize.cpp -pthread -o Alex-pi
```

### Run
Run the generated executable file to start sending commands to the Arduino to operate Alex:
```bash
./Alex-pi
```