#!/usr/bin/env python3

from migen import *
from migen.genlib.io import CRG
from migen.genlib.cdc import MultiReg

import nexys4ddr as tarjeta
#import c4e6e10 as tarjeta

from litex.soc.integration.soc_core import *
from litex.soc.integration.builder import *
from litex.soc.interconnect.csr import *

from litex.soc.cores.uart import UARTWishboneBridge
from litex.soc.cores.uart import UARTPHY, UART

from litex.soc.cores import gpio
from module import rgbled
from module import sevensegment
from module import vgacontroller
#from module import camara
from module import ultraSound
from module import PWMUS
from module import infraRed
from module import wheels
from module import i2c




# BaseSoC ------------------------------------------------------------------------------------------

class BaseSoC(SoCCore):
	def __init__(self):
		platform = tarjeta.Platform()
		
		## add source verilog

		
		platform.add_source("module/verilog/divFreq.v")
		platform.add_source("module/verilog/ultraSound.v")
		platform.add_source("module/verilog/PWMUS.v")
		platform.add_source("module/verilog/infraRed.v")
		platform.add_source("module/verilog/wheels.v")

		#platform.add_source("module/verilog/test_cam.v")
		#platform.add_source("module/verilog/cam_read.v")
		#platform.add_source("module/verilog/buffer_ram_dp.v")
		#platform.add_source("module/verilog/clk24_25_nexys4.v")
		#platform.add_source("module/verilog/image.men")



		# SoC with CPU
		SoCCore.__init__(self, platform,
 			cpu_type="picorv32",
#			cpu_type="vexriscv",
			clk_freq=100e6,
			integrated_rom_size=0x6000,
			integrated_main_ram_size=16*1024)
		# Clock Reset Generation
		self.submodules.crg = CRG(platform.request("clk"), ~platform.request("cpu_reset"))

		# Leds
		SoCCore.add_csr(self,"leds")
		user_leds = Cat(*[platform.request("led", i) for i in range(10)])
		self.submodules.leds = gpio.GPIOOut(user_leds)
		
		# Switchs
		SoCCore.add_csr(self,"switchs")
		user_switchs = Cat(*[platform.request("sw", i) for i in range(8)])
		self.submodules.switchs = gpio.GPIOIn(user_switchs)
		
		# Buttons
		SoCCore.add_csr(self,"buttons")
		user_buttons = Cat(*[platform.request("btn%c" %c) for c in ['c','r','l']])
		self.submodules.buttons = gpio.GPIOIn(user_buttons)
		
		# 7segments Display
		SoCCore.add_csr(self,"display")
		display_segments = Cat(*[platform.request("display_segment", i) for i in range(8)])
		display_digits = Cat(*[platform.request("display_digit", i) for i in range(8)])
		self.submodules.display = sevensegment.SevenSegment(display_segments,display_digits)

		# RGB leds
		SoCCore.add_csr(self,"ledRGB_1")
		self.submodules.ledRGB_1 = rgbled.RGBLed(platform.request("ledRGB",1))
		
		SoCCore.add_csr(self,"ledRGB_2")
		self.submodules.ledRGB_2 = rgbled.RGBLed(platform.request("ledRGB",2))
		
				
		# VGA
		SoCCore.add_csr(self,"vga_cntrl")
		vga_red = Cat(*[platform.request("vga_red", i) for i in range(4)])
		vga_green = Cat(*[platform.request("vga_green", i) for i in range(4)])
		vga_blue = Cat(*[platform.request("vga_blue", i) for i in range(4)])
		self.submodules.vga_cntrl = vgacontroller.VGAcontroller(platform.request("hsync"),platform.request("vsync"), vga_red, vga_green, vga_blue)
		
		
		#camara
		#SoCCore.add_csr(self,"camara_cntrl")
		#cam_data_in = Cat(*[platform.request("cam_data_in", i) for i in range(8)])		
		#self.submodules.camara_cntrl = camara.Camara(platform.request("CAM_xclk"),platform.request("CAM_pclk"),cam_data_in)

		#I2C
		SoCCore.add_csr(self,"i2c_cntrl")
		self.submodules.i2c_cntrl = i2c.I2C(platform.request("i2c"))


		#ultraSound
		SoCCore.add_csr(self,"ultraSound_cntrl")
		self.submodules.ultraSound_cntrl = ultraSound.US(platform.request("echo"), 	platform.request("trig"))
		
		#PWMUS
		SoCCore.add_csr(self,"PWMUS_cntrl")
		self.submodules.PWMUS_cntrl = PWMUS.servoUS(platform.request("servo"))

		#infraRed
		SoCCore.add_csr(self,"IR_cntrl")	
		self.submodules.IR_cntrl = infraRed.infra(platform.request("iR"), platform.request("iRC"), platform.request("iC"), platform.request("iLC"), platform.request("iL"))

		#Wheels
		SoCCore.add_csr(self,"wheels_cntrl")
		right = Cat(*[platform.request("right", i) for i in range(2)])
		left = Cat(*[platform.request("left", i) for i in range(2)])
		self.submodules.wheels_cntrl = wheels.wheels(right, left)

		#UART BLUETHOOT
		from litex.soc.cores import uart
		self.submodules.uart1_phy = uart.UARTPHY(
			pads     = platform.request("uart1"),
			clk_freq = self.sys_clk_freq,
			baudrate = 9600)
		self.submodules.uart1 = ResetInserter()(uart.UART(self.uart1_phy,
			tx_fifo_depth = 16,
			rx_fifo_depth = 16))
		self.csr.add("uart1_phy", use_loc_if_exists=True)
		self.csr.add("uart1", use_loc_if_exists=True)
		if hasattr(self.cpu, "interrupt"):
			self.irq.add("uart1", use_loc_if_exists=True)
		else:
			self.add_constant("UART_POLLING")


		#UART MP3
		self.submodules.uart2_phy = uart.UARTPHY(
			pads     = platform.request("uart2"),
			clk_freq = self.sys_clk_freq,
			baudrate = 9600)
		self.submodules.uart2 = ResetInserter()(uart.UART(self.uart2_phy,
			tx_fifo_depth = 16,
			rx_fifo_depth = 16))
		self.csr.add("uart2_phy", use_loc_if_exists=True)
		self.csr.add("uart2", use_loc_if_exists=True)
		if hasattr(self.cpu, "interrupt"):
			self.irq.add("uart2", use_loc_if_exists=True)
		else:
			self.add_constant("UART_POLLING")


		#self.add_csr("bluetooth")
		#self.submodules.bluetooth = ResetInserter()(UART(
        #phy=UARTPHY(platform.request("bluetooth", 0), 100e6, baudrate=9600),
        #tx_fifo_depth=16,
        #rx_fifo_depth=16,
        #phy_cd="sys"))
		#self.add_constant("UART_POLLING")

		#SoCCore.add_uart(self, "crossover", baudrate=9600)

		#self.add_csr("bluetooth")
		#self.submodules.bluetooth = UARTWishboneBridge(platform.request("serial"), 100e6)
		#self.add_wb_master(self.bluetooth.wishbone)

		#self.add_constant("UART_POLLING")
		#self.add_csr("bluetooth")

# Build --------------------------------------------------------------------------------------------
if __name__ == "__main__":
	builder = Builder(BaseSoC(),csr_csv="Soc_MemoryMap.csv")
	builder.build()
