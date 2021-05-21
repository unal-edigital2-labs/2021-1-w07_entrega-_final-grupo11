`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    18:31:11 05/20/2021 
// Design Name: 
// Module Name:    divFreq 
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
module divFreq(
    input clk_int,
    output reg clk_out
    );

parameter FREQ=28'h5F5E100;  // Frecuencia entrada 100MHz
parameter FREQ_OUT =24'h1E8480; //Frecuencia de salida 2MHz---1us
parameter MAX_COUNT=FREQ/(2*FREQ_OUT); //Maximo contador

reg [35:0] count; // Count flancos

initial begin
count =0;
clk_out=0;
end

always @(negedge clk_int)begin
  if(count==(MAX_COUNT)) begin
   clk_out=~clk_out; //Cambio del estado del reloj de salida
	count<=1; //Se resetea el contador
  end
   else begin
	 count<=count+1;
	end
end


endmodule
