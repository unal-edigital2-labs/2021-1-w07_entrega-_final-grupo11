from migen import *
from migen.genlib.cdc import MultiReg
from litex.soc.interconnect.csr import *
from litex.soc.interconnect.csr_eventmanager import *

# Modulo Principal
class Camara(Module,AutoCSR):
    def __init__(self, CAM_xclk,CAM_pclk,cam_data_in):
        self.clk = ClockSignal()   
        self.rst = ResetSignal()  
        self.CAM_xclk = CAM_xclk
        
        self.CAM_pclk = CAM_pclk
        self.CAM_vsync= Signal()
        self.CAM_href= Signal()
        self.CAM_px_data = cam_data_in

        self.data_mem = CSRStatus(12)

        self.specials +=Instance("test_cam",
            i_clk = self.clk,
            i_rst = self.rst,
            o_CAM_xclk = self.CAM_xclk,
            i_CAM_pclk = self.CAM_pclk,
            i_CAM_px_data=self.CAM_px_data,
            i_data_mem=self.data_mem.status
        )
        