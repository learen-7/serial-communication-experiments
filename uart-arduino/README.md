# UART Digit Parser — Arduino UNO R4 LED Matrix

A non-blocking UART parser that displays digits on the Arduino UNO R4 WiFi LED matrix, one per second.

## Demo

When a digit is sent to the board, the LED matrix displays it. After one second, the display changes: if another digit was sent, it displays that one; otherwise, it shows the waiting screen (the happy face).

## Protocol

- A valid frame is exactly one digit (0–9) followed by a newline.
- An invalid frame (empty line, two digits, or any other character) shows the sad face and prints "Send a number between 0 and 9 !" on the serial port.
- Valid frames are queued and displayed for one second each.

## How it works

- **Non-blocking loop**: `millis()` instead of `delay()`, so the board keeps receiving and queueing frames even while a digit is on display.
- **State-machine parser**: the parser checks whether each received byte is a digit, the newline delimiter, or an invalid character. When it receives '\n', it validates the frame and queues it. A second digit or any other character makes the frame invalid (sad face + error message).
- **Circular queue (ring buffer)**: the queue holds 32 frames. Both indices wrap around with a modulo, so they never go out of the array bounds. Before enqueueing, the parser checks whether the queue is full; if so, it prints "Queue full, wait a little !" and drops the frame.
- **Producer / consumer**: the serial input produces digits into the queue, the display consumes them at a fixed one-second cadence, so a burst of input is displayed smoothly without losing track.

## Setup

- Hardware: Arduino UNO R4 WiFi (built-in 12×8 LED matrix).
- Open the serial monitor at 115200 baud, line ending set to "New line".

## Testing

All tests are run in the serial monitor at 115200 baud, with the line-ending
setting set to "New line" unless stated otherwise: the "Send" button then
transmits the frame followed by a '\n' automatically.

|Input | Expected behavior |
|-------|-------------------|
| `1` + Send | `1` is displayed, then the happy face after 1 second |
| Send only (empty frame) | Sad face, "Send a number between 0 and 9 !" printed |
| `12` + Send | Sad face, "Send a number between 0 and 9 !" printed |
| `abd` + Send | Sad face, "Send a number between 0 and 9 !" printed |
| `4` without `\n`| Nothing happens (frame not terminated) |
| `4` then `3`, no `\n` | Sad face, "Send a number between 0 and 9 !" printed (two digits in one frame) |
| 40 × (`3` + Send), sent fast | Digits display one per second, no crash; the overflow frames are rejected with "Queue full, wait a little !" (about 8, depending on whether the consumer dequeued during the flood) |
| `8` + Send, then `9` + Send immediately | `8` displays, `9` is queued and displays one second later |

### Automated flood test

A small Python script sends 40 valid frames as fast as the serial line allows —
more than the 32-slot queue can hold — to validate the overflow behavior.
Close the serial monitor first (the port only accepts one client), then run:

```bash
cd tests/
python3 -m venv .venv
source .venv/bin/activate
pip install pyserial
python test_queue.py
```

**Expected result:** the board displays the accepted digits one per second, and the
script prints one "Queue full, wait a little !" per rejected frame (about 8,
depending on whether the consumer dequeued during the flood). The port name
(/dev/ttyACM0 here) may differ depending on the machine.

