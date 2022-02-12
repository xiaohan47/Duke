module exception(result, dest, overflow, Rd, alu_output, opcode, alu_op);

	input [31:0] alu_output;
	input [4:0] opcode;
	input [4:0] Rd;
	input [4:0] alu_op;
	input overflow;
	output reg [4:0] dest;
	output reg [31:0] result;
	

	// assign dest[4:0] = overflow ? 5'd30 : Rd[4:0];

	always @(*) begin
		if (overflow == 1'b1)
			case (opcode)
				5'b00000: begin
					if (alu_op == 5'b0) begin
						result <= 32'd1;
						dest <= 5'd30;
						end
					else if (alu_op == 5'b00001) begin
						result <=  32'd3;
						dest <= 5'd30; 
						end
					else begin
						result <= alu_output;
						dest <= Rd[4:0];
						end
				end
				5'b00101: begin
					result <= 32'd2;
					dest <= 5'd30;
					end

				default: begin
					result <= alu_output;
					dest <= Rd[4:0];
					end

			endcase
		else begin
			result <= alu_output;
			dest <= Rd[4:0];
			end
	end

endmodule