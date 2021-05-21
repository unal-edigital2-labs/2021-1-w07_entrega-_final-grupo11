`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    12:01:20 05/20/2021 
// Design Name: 
// Module Name:    PWM 
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
module PWM(
    input clk,
	 input Enable,
    input [11:0]dutty,
    input [19:0]period,
	 output wire pwm

    );
	 	 
reg [31:0] count=0;


assign 	pwm =  ((Enable)&&(count<dutty)) ? 1:0; //Ciclo util ServoMotor

always @(negedge clk) begin
		
           if(Enable)begin
			    
			    if(count<period) count <= count+1; //Periodo
			    else count <= 0;
	   
	        end
			  
	        else 
			     count <= 0;
   
		
end

endmodule
