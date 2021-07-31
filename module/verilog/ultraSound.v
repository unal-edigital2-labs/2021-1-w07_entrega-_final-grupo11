`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 25.06.2021 11:40:40
// Design Name: 
// Module Name: ultraSound
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


module ultraSound(  input clk, 
                    input init, 
                    input echo,
                    output reg trig,
                    output reg [8:0] distance,
                    output reg done);

reg [14:0]counter = 0;
reg echoStart = 0;
reg [1:0]status = 0;
wire newCLK;

divFreq freq1(clk, newCLK, 'd50);

parameter Start = 0, Pulse = 'd1, Echo = 'd2, End = 'd3;
always @ (posedge newCLK)begin

    case(status)
        Start:  begin
                    if(init) begin
                        done = 0;
                        counter = 0;
                        distance = 0;
                        status = Pulse;
                    end
                end
        Pulse:  begin
                    trig = 1'b1;
                    counter = counter + 1'b1;
                        if(counter == 'd11)begin
                            trig = 0;
                            counter = 0;
                            status = Echo;
                        end
                end
        Echo:   begin
                      if(echo)begin
                        echoStart = 1;
                        counter = counter + 1'b1;
                      end
                      if(echo == 0 && echoStart == 1)begin
                        
                        if(counter == 0) status = Pulse;
                        else begin
                            distance = counter/'d58;
                            status = Start;
                            done = 1;
                        end
                      end
                end
    endcase 
end

endmodule
