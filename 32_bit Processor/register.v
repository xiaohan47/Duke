module register(Q, D, clk, enable, reset);

	input [31:0] D;
	input clk, enable, reset;
	output [31:0] Q;

	generate
		genvar i;
		for (i = 0; i < 32; i = i + 1) begin: create_dffe
			dffe_ref dff(Q[i], D[i], clk, enable, reset);
		end
	endgenerate
endmodule