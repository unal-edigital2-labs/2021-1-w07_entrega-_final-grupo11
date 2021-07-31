#!/usr/bin/env python3

from migen import *
from migen.genlib.io import CRG
from migen.genlib.cdc import MultiReg

import nexys4ddr as tarjeta
#import c4e6e10 as tarjeta

from litex.soc.integration.soc_core import *
from litex.soc.integration.builder import *
from litex.soc.interconnect.csr import *

from litex.soc.cores import gpio
from module import rgbled
from module import sevensegment
from module import vgacontroller
from module import camara
from module import ultraSound
from module import PWMUS
from module import infraRed
from module import wheels




# BaseSoC ------------------------------------------------------------------------------------------

class BaseSoC(SoCCore):
	def __init__(self):
		platform = tarjeta.Platform()
		
		## add source verilog

		platform.add_source("module/verilog/camara.v")
		platform.add_source("module/verilog/divFreq.v")
		platform.add_source("module/verilog/ultraSound.v")
		platform.add_source("module/verilog/PWMUS.v")
		platform.add_source("module/verilog/infraRed.v")
		platform.add_source("module/verilog/wheels.v")


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
		SoCCore.add_csr(self,"camara_cntrl")
		SoCCore.add_interrupt(self,"camara_cntrl")
		cam_data_in = Cat(*[platform.request("cam_data_in", i) for i in range(8)])		
		self.submodules.camara_cntrl = camara.Camara(platform.request("cam_xclk"),platform.request("cam_pclk"),cam_data_in)

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
		self.submodules.wheels_cntrl = wheels.wheels(platform.request("right"), platform.request("left"))

# Build --------------------------------------------------------------------------------------------
if __name__ == "__main__":
	builder = Builder(BaseSoC(),csr_csv="Soc_MemoryMap.csv")
	builder.build()
