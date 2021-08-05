from migen import *
from migen.genlib.cdc import MultiReg
from litex.soc.interconnect.csr import *
from litex.soc.interconnect.csr_eventmanager import *

class wheels(Module, AutoCSR):
    def __init__(self, right, left):
            self.clk = ClockSignal()
            self.state = CSRStorage(3)
            self.right = right
            self.left = left

            self.specials += Instance("wheels",
                i_clk = self.clk,
                i_state = self.state.storage,
                o_right = self.right,
                o_left = self.left)