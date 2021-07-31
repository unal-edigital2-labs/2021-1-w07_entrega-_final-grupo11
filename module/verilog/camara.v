`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    11:14:22 12/02/2019 
// Design Name: 
// Module Name:    cam_read 
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
module camara #(
		parameter AW = 15 // Cantidad de bits  de la dirección 
		)(
		input clk,
		input rst,
		output xclk,
		input pclk,
		input [7:0] px_data,
		output done,
		input [AW-1:0] mem_px_addr,
		output [7:0]  mem_px_data
		/* incluir el resto de señales necesarias*/
   );
	

/******************************************************************
HDL EJEMPLO  SE DEBE MODIFICAR 
******************************************************************/
	
	reg [26:0] cfreq=0;
	always @(posedge clk) begin
  		if(rst==1) begin
			cfreq <= 0;
		end else begin
			cfreq <=cfreq+1;
		end
	end
	assign xclk = cfreq[25];
	assign done = pclk;
	assign mem_px_data =px_data;

/********************************************************************************
Por favor colocar en este archivo el desarrollo realizado por el grupo para la 
captura de datos de la camara 
debe tener en cuenta el nombre de las entradas  y salidad propuestas 
********************************************************************************/

endmodule
