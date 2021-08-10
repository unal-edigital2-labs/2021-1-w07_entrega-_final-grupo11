## Clock signal
##Bank = 35, Pin name = IO_L12P_T1_MRCC_35,					Sch name = CLK100MHZ
set_property PACKAGE_PIN E3 [get_ports clk]							
	set_property IOSTANDARD LVCMOS33 [get_ports clk]
	create_clock -add -name sys_clk_pin -period 10.00 -waveform {0 5} [get_ports clk]

set_property CLOCK_DEDICATED_ROUTE FALSE [get_nets CAM_pclk_IBUF]


##Bank = 14, Pin name = IO_25_14,							Sch name = BTNR
set_property PACKAGE_PIN R10 [get_ports rst]						
	set_property IOSTANDARD LVCMOS33 [get_ports rst]


##Pmod Header JA
##Bank = 15, Pin name = IO_L1N_T0_AD0N_15,					Sch name = JA1
set_property PACKAGE_PIN B13 [get_ports {CAM_px_data[0]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {CAM_px_data[0]}]
##Bank = 15, Pin name = IO_L5N_T0_AD9N_15,					Sch name = JA2
set_property PACKAGE_PIN F14 [get_ports {CAM_px_data[2]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {CAM_px_data[2]}]
##Bank = 15, Pin name = IO_L16N_T2_A27_15,					Sch name = JA3
set_property PACKAGE_PIN D17 [get_ports {CAM_px_data[4]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {CAM_px_data[4]}]
##Bank = 15, Pin name = IO_L16P_T2_A28_15,					Sch name = JA4
set_property PACKAGE_PIN E17 [get_ports {CAM_px_data[6]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {CAM_px_data[6]}]
##Bank = 15, Pin name = IO_0_15,								Sch name = JA7
set_property PACKAGE_PIN G13 [get_ports {CAM_px_data[1]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {CAM_px_data[1]}]
##Bank = 15, Pin name = IO_L20N_T3_A19_15,					Sch name = JA8
set_property PACKAGE_PIN C17 [get_ports {CAM_px_data[3]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {CAM_px_data[3]}]
##Bank = 15, Pin name = IO_L21N_T3_A17_15,					Sch name = JA9
set_property PACKAGE_PIN D18 [get_ports {CAM_px_data[5]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {CAM_px_data[5]}]
##Bank = 15, Pin name = IO_L21P_T3_DQS_15,					Sch name = JA10
set_property PACKAGE_PIN E18 [get_ports {CAM_px_data[7]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {CAM_px_data[7]}]

##Pmod Header JB
##Bank = 15, Pin name = IO_L15N_T2_DQS_ADV_B_15,				Sch name = JB1
set_property PACKAGE_PIN G14 [get_ports {CAM_xclk}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {CAM_xclk}]
##Bank = 14, Pin name = IO_L13P_T2_MRCC_14,					Sch name = JB2
set_property PACKAGE_PIN P15 [get_ports {CAM_href}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {CAM_href}]
##Bank = 14, Pin name = IO_L21N_T3_DQS_A06_D22_14,			Sch name = JB3
set_property PACKAGE_PIN V11 [get_ports {CAM_pwdn}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {CAM_pwdn}]
##Bank = CONFIG, Pin name = IO_L16P_T2_CSI_B_14,				Sch name = JB4
set_property PACKAGE_PIN V15 [get_ports {CAM_reset}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {CAM_reset}]
##Bank = 15, Pin name = IO_25_15,							Sch name = JB7
set_property PACKAGE_PIN K16 [get_ports {CAM_pclk}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {CAM_pclk}]
##Bank = CONFIG, Pin name = IO_L15P_T2_DQS_RWR_B_14,			Sch name = JB8
set_property PACKAGE_PIN R16 [get_ports {CAM_vsync}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {CAM_vsync}]
##Bank = 14, Pin name = IO_L24P_T3_A01_D17_14,				Sch name = JB9
#set_property PACKAGE_PIN T9 [get_ports {JB[6]}]					
	#set_property IOSTANDARD LVCMOS33 [get_ports {JB[6]}]
##Bank = 14, Pin name = IO_L19N_T3_A09_D25_VREF_14,			Sch name = JB10 
#set_property PACKAGE_PIN U11 [get_ports {JB[7]}]					
	#set_property IOSTANDARD LVCMOS33 [get_ports {JB[7]}]
 


 

##VGA Connector
##Bank = 35, Pin name = IO_L8N_T1_AD14N_35,					Sch name = VGA_R0
set_property PACKAGE_PIN A3 [get_ports {VGA_R[0]}]				
	set_property IOSTANDARD LVCMOS33 [get_ports {VGA_R[0]}]
##Bank = 35, Pin name = IO_L7N_T1_AD6N_35,					Sch name = VGA_R1
set_property PACKAGE_PIN B4 [get_ports {VGA_R[1]}]				
	set_property IOSTANDARD LVCMOS33 [get_ports {VGA_R[1]}]
##Bank = 35, Pin name = IO_L1N_T0_AD4N_35,					Sch name = VGA_R2
set_property PACKAGE_PIN C5 [get_ports {VGA_R[2]}]				
	set_property IOSTANDARD LVCMOS33 [get_ports {VGA_R[2]}]
##Bank = 35, Pin name = IO_L8P_T1_AD14P_35,					Sch name = VGA_R3
set_property PACKAGE_PIN A4 [get_ports {VGA_R[3]}]				
	set_property IOSTANDARD LVCMOS33 [get_ports {VGA_R[3]}]
##Bank = 35, Pin name = IO_L2P_T0_AD12P_35,					Sch name = VGA_B0
set_property PACKAGE_PIN B7 [get_ports {VGA_B[0]}]				
	set_property IOSTANDARD LVCMOS33 [get_ports {VGA_B[0]}]
##Bank = 35, Pin name = IO_L4N_T0_35,						Sch name = VGA_B1
set_property PACKAGE_PIN C7 [get_ports {VGA_B[1]}]				
	set_property IOSTANDARD LVCMOS33 [get_ports {VGA_B[1]}]
##Bank = 35, Pin name = IO_L6N_T0_VREF_35,					Sch name = VGA_B2
set_property PACKAGE_PIN D7 [get_ports {VGA_B[2]}]				
	set_property IOSTANDARD LVCMOS33 [get_ports {VGA_B[2]}]
##Bank = 35, Pin name = IO_L4P_T0_35,						Sch name = VGA_B3
set_property PACKAGE_PIN D8 [get_ports {VGA_B[3]}]				
	set_property IOSTANDARD LVCMOS33 [get_ports {VGA_B[3]}]
##Bank = 35, Pin name = IO_L1P_T0_AD4P_35,					Sch name = VGA_G0
set_property PACKAGE_PIN C6 [get_ports {VGA_G[0]}]				
	set_property IOSTANDARD LVCMOS33 [get_ports {VGA_G[0]}]
##Bank = 35, Pin name = IO_L3N_T0_DQS_AD5N_35,				Sch name = VGA_G1
set_property PACKAGE_PIN A5 [get_ports {VGA_G[1]}]				
	set_property IOSTANDARD LVCMOS33 [get_ports {VGA_G[1]}]
##Bank = 35, Pin name = IO_L2N_T0_AD12N_35,					Sch name = VGA_G2
set_property PACKAGE_PIN B6 [get_ports {VGA_G[2]}]				
	set_property IOSTANDARD LVCMOS33 [get_ports {VGA_G[2]}]
##Bank = 35, Pin name = IO_L3P_T0_DQS_AD5P_35,				Sch name = VGA_G3
set_property PACKAGE_PIN A6 [get_ports {VGA_G[3]}]				
	set_property IOSTANDARD LVCMOS33 [get_ports {VGA_G[3]}]
##Bank = 15, Pin name = IO_L4P_T0_15,						Sch name = VGA_HS
set_property PACKAGE_PIN B11 [get_ports VGA_Hsync_n]						
	set_property IOSTANDARD LVCMOS33 [get_ports VGA_Hsync_n]
##Bank = 15, Pin name = IO_L3N_T0_DQS_AD1N_15,				Sch name = VGA_VS
set_property PACKAGE_PIN B12 [get_ports VGA_Vsync_n]	
	set_property IOSTANDARD LVCMOS33 [get_ports VGA_Vsync_n]					