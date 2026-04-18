# Reaction Time Measurement System (HCS12)

## Overview
Developed an embedded system using the HCS12 microcontroller to measure human reaction time based on user input.

## Features
- Button-triggered reaction test
- Accurate timing using hardware timers
- Serial output of measured reaction time

## Hardware
- Dragon12-Light Rev D board
- HCS12 microcontroller
- Push buttons (SW5 to start, SW2 to stop)

## Tools Used
- CodeWarrior
- C programming

## Implementation
- Configured PLL for 24 MHz system clock
- Used input capture/output compare for timing
- Implemented SCI (UART) for serial communication output

## Testing & Validation
- Verified timing accuracy using multiple test runs
- Observed correct start/stop behavior via button inputs
- Debugged timing inconsistencies and input handling

## Challenges
- Synchronizing button input with timer start/stop
- Ensuring accurate timing measurement with hardware interrupts

## What I Learned
- Embedded system timing and interrupts
- Serial communication (SCI)
- Debugging real hardware behavior
