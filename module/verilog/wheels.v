`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 27.06.2021 01:28:35
// Design Name: 
// Module Name: wheels
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module wheels(input clk,
            input [1:0]state,
            output reg right,
            output reg left);
    
always@(posedge clk)begin	
	case(state)
        2'b00:  begin
                    right = 1;
                    left  = 1;
                end
        2'b01:  begin
                    right = 0;
                    left  = 1;
                end
        2'b10:  begin
                    right = 1;
                    left  = 0;
                end
        2'b11:  begin
                    right = 0;
                    left  = 0;
                end
                    
	endcase
end
endmodule
