from migen import *
from migen.genlib.cdc import MultiReg
from litex.soc.interconnect.csr import *
from litex.soc.interconnect.csr_eventmanager import *

# Modulo Principal
class Camara(Module,AutoCSR):
    def __init__(self, xclk,pclk,cam_data_in):
        self.clk = ClockSignal()   
        self.rst = ResetSignal()  
        self.xclk = xclk
        
        self.pclk = pclk
        self.vsync= Signal()
        self.href= Signal()
        self.px_data = cam_data_in
        
        self.done= CSRStatus()
        
        self.mem_px_addr = CSRStorage(15)
        self.mem_px_data = CSRStatus(8)  

        self.specials +=Instance("camara",
            i_clk = self.clk,
            i_rst = self.rst,
            o_xclk = self.xclk,
            i_pclk = self.pclk,
            i_px_data=self.px_data,
            o_done =self.done.status,
            i_mem_px_addr=self.mem_px_addr.storage,
            o_mem_px_data= self.mem_px_data.status,
        )
        
        
       
        self.submodules.ev = EventManager()
        self.ev.ok = EventSourceProcess()
        self.ev.finalize()
 
        self.ev.ok.trigger.eq(self.done.status)