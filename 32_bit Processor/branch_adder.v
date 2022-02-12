module branch_adder(in1, in2, res);
	input [31:0] in1, in2;
	output [31:0] res;

	assign res = in1 + in2;
endmodule