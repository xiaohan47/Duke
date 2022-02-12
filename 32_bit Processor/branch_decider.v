module branch_decider(opcode, neq, lt, res);
	input [4:0] opcode;
	input neq, lt;
	output reg res;
initial begin
	res <= 1'b0;
end

always @(*) begin
	case(opcode)
		5'b00010: begin
			if (neq == 1'b1) res <= 1'b1;
			else res <= 1'b0;
		end

		5'b00110: begin
			if (lt == 1'b1) res <= 1'b1;
			else res <= 1'b0;
		end
		default: begin
			res <= 1'b0;
		end
	endcase
end
endmodule