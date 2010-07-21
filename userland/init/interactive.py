import keyboard
import code
import sys

class Console(code.InteractiveConsole):
    def raw_input(self, prompt=""):
        if prompt != "":
            sys.stdout.write(prompt)
            sys.stdout.flush()
        
        line = keyboard.input_line()
        return line

    def interact(self, banner=None):
        cprt = 'Type "help", "copyright", "credits" or "license" for more information.'
        if banner == None:
            banner = "Python %s on ZSOS\n%s" % \
                       (sys.version, cprt)

        code.InteractiveConsole.interact(self, banner)
