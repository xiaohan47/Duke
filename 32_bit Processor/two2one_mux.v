module two2one_mux(Q, in1, in2, rdst);
	input [4:0] in1, in2;
	input rdst;
	output [4:0] Q;
	
	assign Q = rdst ? in2 : in1;

//	always begin
//	//1 goes immed
//		if (rdst == 1'b1) begin
//			Q <= in2;
//		end
//		else begin
//			Q <= in1;
//		end
//	end
endmodule