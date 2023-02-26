# TinyCols
A jewels matching game for the Linux terminal.

![Screenshot](media/screen.png)

## Features
- [x] Single player mode
- [x] Levels (infinite mode)
- [x] Scoring
- [x] Color
- [ ] Configuration menu
- [ ] Hi-score table
- [ ] Flash columns mode
- [ ] 2 player vs. mode
- [ ] Computer vs. mode
- [ ] Internet vs. mode
- [ ] A.I. vs. mode

## Controls
| key | actions                      |
|-----|------------------------------|
| →   | move piece right             |
| ←   | move piece left              |
| ↓   | move piece down (accelerate) |
| ↑   | rotate jewels upwards        |

## Build

### Pre-requisites
 - `ncurses-devel`
 - A C compiler

### Instructions
Run `make`, then `./build/tinycols`

## Test
Run `make test`, then `./build/test`

