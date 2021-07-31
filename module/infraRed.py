from migen import *
from migen.genlib.cdc import MultiReg
from litex.soc.interconnect.csr import *
from litex.soc.interconnect.csr_eventmanager import *

class infra(Module, AutoCSR):
    def __init__(self, iR, iRC, iC, iLC, iL):

            self.iL = iL
            self.iLC = iLC
            self.iC = iC
            self.iRC = iRC
            self.iR = iR

            self.L = CSRStatus(1)
            self.LC = CSRStatus(1)
            self.C = CSRStatus(1)
            self.RC = CSRStatus(1)
            self.R = CSRStatus(1)

            self.specials += Instance("infraRed",
                i_iL = self.iL,
                i_iLC = self.iLC,
                i_iC = self.iC,
                i_iRC = self.iRC,
                i_iR = self.iR,
                o_L = self.L.status,
                o_LC = self.LC.status,
                o_C = self.C.status,
                o_RC = self.RC.status,
                o_R = self.R.status)