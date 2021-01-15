# Smally

Its actually a "smart" (well, not really :v) door lock system using combination of sound sensor and ultrasonic sensor to create password-like door lock system that enables us to open the door lock without even touching it 🤯.

This is created by [@frhngm](https://github.com/frhngm), [@KHYIN](https://github.com/KHYIN31), and [@fakhrip](https://github.com/fakhrip).

All the simulation were created using proteus and the project including all required sensors are available in the `proteus_simulation_files.zip`.

Code is available in `arduino_code`, and its pretty simple, it will just save a sequence of distance calculated by the ultrasonic sensor in each clap, and once the whole sequence completed, it will check if the sequence are equal with the hardcoded pass or not.