# TinyCols
A jewels matching game for the Unix terminal.

![Screenshot](media/video.gif)

## Features
- [x] Single player mode
- [x] Levels (infinite mode)
- [x] Scoring
- [x] Color
- [ ] Magic jewel
- [ ] NO_COLOR
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
| ↑ a | rotate jewels upwards        |
| z   | rotate jewels downwards      |

## Build

### Pre-requisites
 - `make`
 - `gcc`
 - `ncurses-devel`

For debugging:
 - libasan
 - libubsan

### Instructions
Run `make`.

## Run
Run `./build/tinycols -h` for usage information.

## Test
Run `make test && ./build/test`.

