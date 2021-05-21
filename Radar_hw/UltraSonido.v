`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    13:09:52 05/20/2021 
// Design Name: 
// Module Name:    UltraSonido 
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
module UltraSonido(
    input clk,
    input Init,
	 input Echo,
    input [7:0]Period_US,
    input [3:0]Dutty_US,
    output reg [3:0]dutty,
    output reg [19:0]period,
	 output reg [7:0] leds,
	 output reg Done
    );


reg [15:0] count_Echo=0;
reg [15:0] Total_Echo;
reg [15:0] Salida_Echo;
wire [11:0] Distancia;


always @(clk)begin
 
 if(Init) begin
 
    period <= Period_US*(12'h3E8);
	 dutty <= Dutty_US;
	 
 end
end

assign Distancia = (Salida_Echo*10)/58;



always @(negedge clk)begin
	
	   if(Init)begin
		   			
			if(Echo) 
				count_Echo <= count_Echo +1; 
			else 
				Total_Echo <= count_Echo;
			
   	   if(Total_Echo>0) begin  
				Salida_Echo <= Total_Echo; 
				count_Echo <= 0; 
				Done <= 1;
			end
		end
		
		else 
		   begin
			Done <= 0;

			end
  end

always @(Distancia) begin
 if(Distancia <= 30) 
      leds <= 8'b00000000;
	  
 else if(Distancia <= 60) 
      leds <= 8'b00000011;
	  
 else if(Distancia <= 90)
      leds <= 8'b00000111;
	  
 else if(Distancia <= 120)
      leds <= 8'b00001111;
	  
 else if(Distancia <= 150)
      leds <= 8'b00011111;
	  
 else if(Distancia <= 180)
      leds <= 8'b00111111;
	  
 else if(Distancia <= 210)
      leds <= 8'b01111111;
	  
 else  leds <= 8'b11111111;

end

endmodule
