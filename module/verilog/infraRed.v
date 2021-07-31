`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 25.06.2021 11:40:40
// Design Name: 
// Module Name: infraRed
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


module infraRed(input iR, 
input iRC, 
input iC, 
input iLC, 
input iL, 
output reg L, 
output reg LC, 
output reg C, 
output reg RC, 
output reg R);

always @* begin
    L = iL;
    LC = iLC;
    C = iC;
    RC = iRC;
    R = iR;
end

endmodule
