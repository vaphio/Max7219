This is routine for 8x8dot x 4 matrix LED module using 4 sets of MAX7219 driver.
Most of ready-made libraries have too much features which I don't need.
So I made routine which has minimum features for this LED module.
Everything is one *.ino file except character code of "hankaku_v.h" which is 5x7 dot alphabet pattern.
- firstTest : Just on/off of each module for test understanding driver function. Although "hankaku_v.h" is included,
  it is not used.
- dspAlfa_test : display alphabet character including numbers using "hankaku_v.h"
- dspAlfa_left_scroll : add scroll function additionally display long message.
