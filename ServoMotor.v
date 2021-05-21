`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    12:41:00 05/20/2021 
// Design Name: 
// Module Name:    ServoMotor 
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
module ServoMotor(
    input clk,
    input Init,
    input [7:0]Period_SM,
    input [3:0]Dutty_SM,
	 output reg [11:0]dutty,
	 output reg [19:0]period
    );
parameter FREQ=12'h3E8; //1k  16'h4E20
parameter FREQ100=8'h64; //100

always @(posedge clk)begin
 if(Init)begin
   dutty <= Dutty_SM*FREQ100;
   period <= Period_SM*FREQ;
 
 end

end

endmodule
