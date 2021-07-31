from migen import *
from migen.genlib.cdc import MultiReg
from litex.soc.interconnect.csr import *
from litex.soc.interconnect.csr_eventmanager import *

class servoUS(Module, AutoCSR):
    def __init__(self, servo):
            self.clk = ClockSignal()
            self.pos = CSRStorage(2)
            self.servo = servo

            self.specials += Instance("PWMUS",
                i_clk = self.clk,
                i_pos = self.pos.storage,
                o_servo = self.servo)