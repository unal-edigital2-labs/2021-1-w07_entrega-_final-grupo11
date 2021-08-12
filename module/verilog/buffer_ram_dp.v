`timescale 10ns / 1ns
// Se puede cambiar por `timescale 1ns / 1ps.
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
// # Definir y sobreescribir parÃ¡metros [1, pÃ¡g 211]
// Se usa para anchos de bits parametrizables
// Uso: <name_module> #(a,b,...) <nombre de instanciaciÃ³n> (a.(),b.(),...);
// Al instanciar sin #(a,b,...) se colocan los valores inicialmente definidos.
// AW se calcula como log_2(#pixeles)
// DW bits por cada pixel.

module buffer_ram_dp#(
	parameter AW = 15,		 // Cantidad de bits de la direccion.
	parameter DW = 8,		 // Cantidad de Bits de los datos.
	parameter imageFILE= "/image.men")
	
	(
	input clk_w,     		 // Frecuencia de toma de datos de cada pixel.
	input [AW-1: 0] addr_in, // DirecciÃ³n entrada dada por el capturador.
	input [DW-1: 0] data_in, // Datos que entran de la cÃ¡mara.
	input regwrite,		  	 // Enable.

	input clk_r, 				    // Reloj 25MHz VGA.
	input [AW-1: 0] addr_out, 		// DirecciÃ³n de salida dada por VGA.
	output reg [DW-1: 0] data_out,	// Datos enviados a la VGA.
	//input reset					// De momento no se esta usando.
		// señales del procesamiento
	input  [AW-1: 0]proc_addr_in,
	output reg [DW-1: 0] proc_data_in	
	);

// Calcular el numero de posiciones totales de memoria.
localparam NPOS = 2 ** AW; 			// Memoria.
localparam imaSiz=160*120;
 reg [DW-1: 0] ram [0: NPOS-1];

// Escritura  de la memoria port 1.
always @(posedge clk_w) begin
       if (regwrite == 1)
// Escribe los datos de entrada en la direcciÃ³n que addr_in se lo indique.
             ram[addr_in] <= data_in;
end

// Lectura  de la memoria port 2.
always @(posedge clk_r) begin
// Se leen los datos de las direcciones addr_out y se sacan en data_out.
		data_out <= ram[addr_out];
end


always @(proc_addr_in) begin
proc_data_in<=ram[proc_addr_in];
end


initial begin
// Lee en hexadecimal (readmemb lee en binario) dentro de ram [1, pÃ¡g 217].
	$readmemh(imageFILE, ram);
	// En la posición n+1 (160*120) se guarda el color negro
	ram[imaSiz] = 8'h0;
end
endmodule

