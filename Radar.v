`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    12:33:38 05/20/2021 
// Design Name: 
// Module Name:    Radar 
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
module Radar(
    input Enable_SM,
    input Enable_US,
    input clk,
    input Echo,
    input [7:0]Period,
    input [3:0]Dutty,
    output reg Trigger,
	 output [7:0]Led,
    output reg Motor,

    output Done
    );


reg [11:0]dutty;
reg [19:0]period;
reg Enable;

wire [11:0]dutty_sm;
wire [19:0]period_sm;

wire [3:0]dutty_us;
wire [19:0]period_us;

wire pwm;

// Instantiate clk 1us
divFreq clk_1us (clk, clk_out);

// Instantiate PWM
PWM Signal_pwm (
    .clk(clk_out), 
    .Enable(Enable), 
    .dutty(dutty), 
    .period(period), 
	 .pwm(pwm)
    );
always @(negedge clk_out)begin
		if(Enable_SM) begin
            dutty <= dutty_sm;
				period <= period_sm;
				Enable <= Enable_SM;
				Motor <= pwm;
				Trigger <= 0;
				
       end
		 else if(Enable_US) begin

             dutty <= {12'h0,dutty_us};
				 period <= period_us;
				 Enable <= Enable_US;
				 Motor <= 0;
				 Trigger <= pwm;
	 
        end
		  else begin
		      dutty <= 12'h0;
				period <= 20'h0;
				Enable <= 0;
				Motor <= 0;
				Trigger <= 0;
		  end
end
// Instantiate ServoMotor
ServoMotor SM (
    .clk(clk_out), 
    .Init(Enable_SM), 
    .Period_SM(Period), 
    .Dutty_SM(Dutty),  
    .dutty(dutty_sm),
    .period(period_sm)
    );

// Instantiate UltraSonido
UltraSonido US (
    .clk(clk_out), 
    .Init(Enable_US), 
    .Echo(Echo), 
    .Period_US(Period), 
    .Dutty_US(Dutty),  
    .dutty(dutty_us), 
    .period(period_us),
    .leds(Led),
    .Done(Done)
    );
endmodule
