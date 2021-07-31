#
# This file is part of LiteX.
#
# Copyright (c) 2014-2019 Florent Kermarrec <florent@enjoy-digital.fr>
# SPDX-License-Identifier: BSD-2-Clause

from litex.build.generic_platform import *
from litex.build.altera import AlteraPlatform
from litex.build.altera.programmer import USBBlaster

# IOs ----------------------------------------------------------------------------------------------

_io = [
    # Clk
    ("clk", 0, Pins("23"), IOStandard("3.3-V LVTTL")),

    ("cpu_reset", 0, Pins("86"), IOStandard("3.3-V LVTTL")),
 
    # Leds
    ("led", 0, Pins("72"), IOStandard("3.3-V LVTTL")),
    ("led", 1, Pins("73"), IOStandard("3.3-V LVTTL")),
    ("led", 2, Pins("74"), IOStandard("3.3-V LVTTL")),
    ("led", 3, Pins("80"), IOStandard("3.3-V LVTTL")),
    ("led", 4, Pins("83"),  IOStandard("3.3-V LVTTL")),
    ("led", 5, Pins("84"),  IOStandard("3.3-V LVTTL")),
    ("led", 6, Pins("77"),  IOStandard("3.3-V LVTTL")),
    ("led", 7, Pins("76"),  IOStandard("3.3-V LVTTL")),
    ("led", 8, Pins("75"),  IOStandard("3.3-V LVTTL")),
    ("led", 9, Pins("71"),  IOStandard("3.3-V LVTTL")),

    # Button
    ("btnc", 0, Pins("87"), IOStandard("3.3-V LVTTL")),
    ("btnl", 1, Pins("91"),  IOStandard("3.3-V LVTTL")),
    ("btnr", 2, Pins("90"),  IOStandard("3.3-V LVTTL")),

    # Switches
    ("sw", 0, Pins("66"),  IOStandard("3.3-V LVTTL")),
    ("sw", 1, Pins("67"),  IOStandard("3.3-V LVTTL")),
    ("sw", 2, Pins("68"),  IOStandard("3.3-V LVTTL")),
    ("sw", 3, Pins("58"), IOStandard("3.3-V LVTTL")),
    ("sw", 4, Pins("59"),  IOStandard("3.3-V LVTTL")),
    ("sw", 5, Pins("60"),  IOStandard("3.3-V LVTTL")),
    ("sw", 6, Pins("64"),  IOStandard("3.3-V LVTTL")),
    ("sw", 7, Pins("65"), IOStandard("3.3-V LVTTL")),

    ("ledRGB", 1,
        Subsignal("r", Pins("N15")),
        Subsignal("g", Pins("M16")),
        Subsignal("b", Pins("R12")),
        IOStandard("3.3-V LVTTL")),

    ("ledRGB", 2,
        Subsignal("r", Pins("N16")),
        Subsignal("g", Pins("R11")),
        Subsignal("b", Pins("G14")),
        IOStandard("3.3-V LVTTL")),
  
    ("display_digit",  0, Pins("128"), IOStandard("3.3-V LVTTL")),
    ("display_digit",  1, Pins("129"), IOStandard("3.3-V LVTTL")),
    ("display_digit",  2, Pins("132"), IOStandard("3.3-V LVTTL")),
    ("display_digit",  3, Pins("133"), IOStandard("3.3-V LVTTL")),
    ("display_digit",  4, Pins("135"), IOStandard("3.3-V LVTTL")),
    ("display_digit",  5, Pins("136"), IOStandard("3.3-V LVTTL")),
    ("display_digit",  6, Pins("137"), IOStandard("3.3-V LVTTL")),
    ("display_digit",  7, Pins("138"), IOStandard("3.3-V LVTTL")),
    ("display_segment", 0, Pins("127"), IOStandard("3.3-V LVTTL")),
    ("display_segment", 1, Pins("126"), IOStandard("3.3-V LVTTL")),
    ("display_segment", 2, Pins("125"), IOStandard("3.3-V LVTTL")),
    ("display_segment", 3, Pins("124"), IOStandard("3.3-V LVTTL")),
    ("display_segment", 4, Pins("121"), IOStandard("3.3-V LVTTL")),
    ("display_segment", 5, Pins("120"), IOStandard("3.3-V LVTTL")),
    ("display_segment", 6, Pins("119"), IOStandard("3.3-V LVTTL")),
    ("display_segment", 7, Pins("115"), IOStandard("3.3-V LVTTL")),
    
     ("vga_red", 0, Pins("A3"), IOStandard("LVCMOS33")),
    ("vga_red", 1, Pins("B4"), IOStandard("LVCMOS33")),
    ("vga_red", 2, Pins("C5"), IOStandard("LVCMOS33")),
    ("vga_red", 3, Pins("A4"), IOStandard("LVCMOS33")),
    ("vga_green", 0, Pins("C6"), IOStandard("LVCMOS33")),
    ("vga_green", 1, Pins("A5"), IOStandard("LVCMOS33")),
    ("vga_green", 2, Pins("B6"), IOStandard("LVCMOS33")),
    ("vga_green", 3, Pins("A6"), IOStandard("LVCMOS33")),
    ("vga_blue", 0, Pins("B7"), IOStandard("LVCMOS33")),
    ("vga_blue", 1, Pins("C7"), IOStandard("LVCMOS33")),
    ("vga_blue", 2, Pins("D7"), IOStandard("LVCMOS33")),
    ("vga_blue", 3, Pins("D8"), IOStandard("LVCMOS33")),
    ("hsync", 0, Pins("B11"), IOStandard("LVCMOS33")),
    ("vsync", 0, Pins("B12"), IOStandard("LVCMOS33")),
   
    # Serial
    ("serial", 0,
        # Compatible with cheap FT232 based cables (ex: Gaoominy 6Pin Ftdi Ft232Rl Ft232)
        # GND on JP1 Pin 12.
        Subsignal("tx", Pins("113"), IOStandard("3.3-V LVTTL")), # GPIO_07 (JP1 Pin 10)
        Subsignal("rx", Pins("114"), IOStandard("3.3-V LVTTL"))  # GPIO_05 (JP1 Pin 8)
    ),

    # SDR SDRAM
    ("sdram_clock", 0, Pins("R4"), IOStandard("3.3-V LVTTL")),
    ("sdram", 0,
        Subsignal("a",     Pins(
            "P2 N5 N6 M8 P8 T7 N8 T6",
            "R1 P1 N2 N1 L4")),
        Subsignal("ba",    Pins("M7 M6")),
        Subsignal("cs_n",  Pins("P6")),
        Subsignal("cke",   Pins("L7")),
        Subsignal("ras_n", Pins("L2")),
        Subsignal("cas_n", Pins("L1")),
        Subsignal("we_n",  Pins("C2")),
        Subsignal("dq", Pins(
            "G2 G1 L8 K5 K2 J2 J1 R7",
            "T4 T2 T3 R3 R5 P3 N3 K1")),
        Subsignal("dm", Pins("R6 T5")),
        IOStandard("3.3-V LVTTL")
    ),

    # ECPS
    ("epcs", 0,
        Subsignal("data0", Pins("H2")),
        Subsignal("dclk",  Pins("H1")),
        Subsignal("ncs0",  Pins("D2")),
        Subsignal("asd0",  Pins("C1")),
        IOStandard("3.3-V LVTTL")
    ),

    # I2C
    ("i2c", 0,
        Subsignal("sclk", Pins("F2")),
        Subsignal("sdat", Pins("F1")),
        IOStandard("3.3-V LVTTL")
    ),

    # Accelerometer
    ("acc", 0,
        Subsignal("cs_n", Pins("G5")),
        Subsignal("int",  Pins("M2")),
        IOStandard("3.3-V LVTTL")
    ),

    # ADC
    ("adc", 0,
        Subsignal("cs_n",  Pins("A10")),
        Subsignal("saddr", Pins("B10")),
        Subsignal("sclk",  Pins("B14")),
        Subsignal("sdat",  Pins("A9")),
        IOStandard("3.3-V LVTTL")
    ),

    # GPIOs
    ("gpio_0", 0, Pins(
        "D3 C3  A2  A3  B3  B4  A4  B5",
        "A5 D5  B6  A6  B7  D6  A7  C6",
        "C8 E6  E7  D8  E8  F8  F9  E9",
        "C9 D9 E11 E10 C11 B11 A12 D11",
        "D12 B12"),
        IOStandard("3.3-V LVTTL")
    ),
    ("gpio_1", 0, Pins(
        "F13 T15 T14 T13 R13 T12 R12 T11",
        "T10 R11 P11 R10 N12  P9  N9 N11",
        "L16 K16 R16 L15 P15 P16 R14 N16",
        "N15 P14 L14 N14 M10 L13 J16 K15",
        "J13 J14"),
        IOStandard("3.3-V LVTTL")
    ),
    ("gpio_2", 0, Pins(
        "A14 B16 C14 C16 C15 D16 D15 D14",
        "F15 F16 F14 G16 G15"),
        IOStandard("3.3-V LVTTL")
    ),
]

# Platform -----------------------------------------------------------------------------------------

class Platform(AlteraPlatform):
    default_clk_name   = "clk"
    default_clk_period = 1e9/50e6

    def __init__(self):
        AlteraPlatform.__init__(self, "EP4CE10E22C8", _io)
                                      
    def create_programmer(self):
        return USBBlaster()

    def do_finalize(self, fragment):
        AlteraPlatform.do_finalize(self, fragment)
        self.add_period_constraint(self.lookup_request("clk", loose=True), 1e9/50e6)
