`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// 
// Create Date:    13:34:31 10/22/2019 
// Design Name: 	 Ferney alberto Beltran Molina
// Module Name:    buffer_ram_dp 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module buffer_ram_dp#( 
	parameter AW = 15, 
	parameter DW = 8, 
	parameter   imageFILE= "/image.men")
	(  
	input  clk_w, 
	input  [AW-1: 0] addr_in, 
	input  [DW-1: 0] data_in,
	input  regwrite, 
	
	input  clk_r, 
	input [AW-1: 0] addr_out,
	output reg [DW-1: 0] data_out
	);

localparam NPOS = 2 ** AW; 
localparam imaSiz = 160 * 120;
 reg [DW-1: 0] ram [0: NPOS-1]; 


always @(posedge clk_w) begin 
       if (regwrite == 1) 
             ram[addr_in] <= data_in;
end

always @(posedge clk_r) begin 
		data_out <= ram[addr_out]; 
end


initial begin
	$readmemh(imageFILE, ram);
	ram[imaSiz]=8'h0;	
end

endmodule
