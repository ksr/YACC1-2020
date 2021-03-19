# Highlevel project status & work to be done. Additional detail within the various subdirectories

## Memory Card
Add 4k block select for Hi Ram and Rom both located at 0x8000 to 0xFFFF.<br/>
This will allow mapping any 4k block of either RAM or ROM to be mapped in or out of the address space and
allow memory blocks from devices such as a video card to be mapped into the address space.
Currernly the upper 4k of ROM 0XF000-0XFFFF is forced into address space at 0x0000 upon reset and
is mapped back to 0XF000-0XFFFF after first access, this behavior will continue.

## Sequencer Memory Card
Add second 512k EEPROM

## All Cards
Cleanup chip labeling to follow a standard grid<br/> and ic/discrete labels postion and font.
