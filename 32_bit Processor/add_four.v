module add_four(Q, D);

input [31:0] D;
output [31:0] Q;

//always begin
//	Q <= D + 32'd4;
//end
assign Q = D + 32'd1;

endmodule