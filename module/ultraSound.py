from migen import *
from migen.genlib.cdc import MultiReg
from litex.soc.interconnect.csr import *
from litex.soc.interconnect.csr_eventmanager import *

class US(Module, AutoCSR):
    def __init__(self, echo, trig):
        self.clk = ClockSignal()
        self.init = CSRStorage(1)
        self.echo = echo
        self.trig = trig
        self.distance = CSRStatus(9)
        self.done = CSRStatus(1)

        self.specials += Instance("ultraSound",
            i_clk = self.clk,
            i_init = self.init.storage,
            i_echo = self.echo,
            o_trig = self.trig,
            o_distance = self.distance.status,
            o_done = self.done.status)

