# -*- coding: utf8 -*-
import os
import sys

from scancodestokeycodes import get_key_code_for
from keymap              import symstonums
import keycodestokeysyms

class KeyCodeReader:
    def __init__(self):
        self.buffer   = []

    def _get_scancode(self):
        if not len(self.buffer):
            rv = os.read(1, 256)
            self.buffer.extend([ ord(i) for i in rv ])

        return self.buffer.pop(0)
    
    def read_key_code(self):
        while True:
            code = self._get_scancode()
            extended = False
            if code == 0xE0:
                extended = True
                code = self._get_scancode()
        
            elif code == 0xE1:
                code = self._get_scancode()
                code = self._get_scancode()
                # TODO: handle pause...

            release      = bool(code & 0x80)
            combinedscan = (code & 0x7F) + (0x80 * extended)
            key_code = get_key_code_for(combinedscan)

            if key_code:
                return (key_code, not release)

class KeyboardReader:
    MODIFIERS = {
        'Shift':   1,
        'AltGr':   2,
        'Control': 4,
        'Alt':     8
    }

    def __init__(self):
        self.keycodes = KeyCodeReader()
        self.capslock = False
        self.current_modifiers = 0
        self.modifier_counters = { 1: set(), 2: set(), 4: set(), 8: set() }
    
    def read_key(self):
        while True:
            code, down = self.keycodes.read_key_code()
            keysym_line = keycodestokeysyms.map[code]
            if not keysym_line:
                continue

            keysym = keysym_line[self.current_modifiers]

            if keysym == "CapsLock":
                self.capslock = not self.capslock

            if self.MODIFIERS.has_key(keysym):
                # modifier pressed or released
                modbit = self.MODIFIERS[keysym]
                if down:
                    self.modifier_counters[modbit].add(code)
                    self.current_modifiers |= modbit
                else:
                    # must keep in set to allow for context swiches
                    self.modifier_counters[modbit].remove(code)
                    if not self.modifier_counters[modbit]:
                        self.current_modifiers &= ~modbit
            
            if keysym[0] == '+':
                if self.capslock:
                    # capslock
                    keysym = keysym_line[self.current_modifiers ^ 0x1]

                keysym = keysym.replace('+', '')

            return (code, keysym, down)

class KeyboardAsciiTransformer:
    def __init__(self, reader):
        self.reader = reader

    def read_ascii_sequence(self):
        while True:
            code, keysym, down = self.reader.read_key()
            if down:
                num = symstonums.get(keysym, 0xFFFF)
                if num <= 0xFF:
                    return chr(num)

                # todo: handle specials

keybtrans = KeyboardAsciiTransformer(KeyboardReader())


def input_line():
    buf = []
    key = ""
    while key != "\n":
        key = keybtrans.read_ascii_sequence()
        if key == "\b" or key == chr(127):
            if buf:
                buf.pop()
                sys.stdout.write("\b")
                sys.stdout.flush()
        else:
            buf.append(key)
            sys.stdout.write(key)
            sys.stdout.flush()
             
    return "".join(buf)
