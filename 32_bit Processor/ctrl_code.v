module ctrl_code(opcode,aluctrl, neq, lt, Rwe,ALUinB,DMwe,Rwd,rdst,alu_op, jp, br, jal, jr, bex, setx);
	input [4:0]opcode,aluctrl;
	input neq, lt;
	output reg Rwe,ALUinB,DMwe,Rwd, rdst, jp, br, jal, jr, bex, setx;
	output reg [4:0] alu_op;
	
	always @(*)
		begin
		case(opcode)
			//R type add,sub,adn,or,sll,sra
			5'b00000:
				begin
				Rwe <= 1'b1;
			   ALUinB <= 1'b0;
				DMwe <= 1'b0;
				Rwd <= 1'b0;
				alu_op <= aluctrl;
				rdst <= 1'b0;
				jp <= 1'b0;
				jal <= 1'b0;
				jr <= 1'b0;
				br <= 1'b0;
				bex <= 1'b0;
				setx <= 1'b0;
				end
			//addi
			5'b00101:
				begin
				Rwe <= 1'b1;
			   ALUinB <= 1'b1;
				DMwe <= 1'b0;
				Rwd <= 1'b0;
				alu_op <= 5'b00000;
				rdst <= 1'b1;
				jp <= 1'b0;
				jal <= 1'b0;
				jr <= 1'b0;
				br <= 1'b0;
				bex <= 1'b0;
				setx <= 1'b0;
				end
			//sw
			5'b00111:
				begin
				Rwe <= 1'b0;
			   ALUinB <= 1'b1;
				DMwe <= 1'b1;
				Rwd <= 1'b1;
				alu_op <= 5'b00000;
				rdst <= 1'b1;
				jp <= 1'b0;
				jal <= 1'b0;
				jr <= 1'b0;
				br <= 1'b0;
				bex <= 1'b0;
				setx <= 1'b0;
				end
			//lw
			5'b01000:
				begin
				Rwe <= 1'b1;
			    ALUinB <= 1'b1;
				DMwe <= 1'b0;
				Rwd <= 1'b1;			
				alu_op <= 5'b00000;
				rdst <= 1'b1;
				jp <= 1'b0;
				jal <= 1'b0;
				jr <= 1'b0;
				br <= 1'b0;
				bex <= 1'b0;
				setx <= 1'b0;
				end
			// bne
			5'b00010: begin
				Rwe <= 1'b0;
				ALUinB <= 1'b1;
				DMwe <= 1'b0;
				Rwd <= 1'b0;
				alu_op <= 5'b00000;
				rdst <= 1'b1;
				if (neq == 1'b1)
					br <= 1'b1;
				else
					br <= 1'b0;
				jp <= 1'b0;
				jal <= 1'b0;
				jr <= 1'b0;
				bex <= 1'b0;
				setx <= 1'b0;
			end
			// blt
			5'b00110: begin
				Rwe <= 1'b0;
				ALUinB <= 1'b1;
				DMwe <= 1'b0;
				Rwd <= 1'b0;
				alu_op <= 5'b00000;
				rdst <= 1'b1;
				if (lt == 1'b1)
					br <= 1'b1;
				else
					br <= 1'b0;
				jp <= 1'b0;
				jal <= 1'b0;
				jr <= 1'b0;
				bex <= 1'b0;
				setx <= 1'b0;
			end
			//j
			5'b00001:
				begin
				Rwe <= 1'b0;
			    ALUinB <= 1'b1;
				DMwe <= 1'b0;
				Rwd <= 1'b0;			
				alu_op <= 5'b00000;
				rdst <= 1'b1;
				jp <= 1'b1;
				br <= 1'b0;
				jr <= 1'b0;
				jal <= 1'b0;
				bex <= 1'b0;
				setx <= 1'b0;
				end
			//bex
			5'b10110:
				begin
				Rwe <= 1'b0;
			    ALUinB <= 1'b0;
				DMwe <= 1'b0;
				Rwd <= 1'b0;			
				alu_op <= 5'b00000;
				rdst <= 1'b0;
				jp <= 1'b1;
				br <= 1'b0;
				jr <= 1'b0;
				jal <= 1'b0;
				bex <= 1'b1;
				setx <= 1'b0;
				end
			//setx
			5'b10101:
				begin
				Rwe <= 1'b1;
			    ALUinB <= 1'b0;
				DMwe <= 1'b0;
				Rwd <= 1'b0;			
				alu_op <= 5'b00000;
				rdst <= 1'b0;
				jp <= 1'b0;
				br <= 1'b0;
				jr <= 1'b0;
				jal <= 1'b0;
				bex <= 1'b0;
				setx <= 1'b1;
				end
			//jal
			5'b00011:
				begin
				Rwe <= 1'b1;
			    ALUinB <= 1'b1;
				DMwe <= 1'b0;
				Rwd <= 1'b0;			
				alu_op <= 5'b00000;
				rdst <= 1'b1;
				jp <= 1'b1;
				br <= 1'b0;
				jr <= 1'b0;
				jal <= 1'b1;
				bex <= 1'b0;
				setx <= 1'b0;
				end
			//jr
			5'b00100:
				begin
				Rwe <= 1'b0;
			    ALUinB <= 1'b0;
				DMwe <= 1'b0;
				Rwd <= 1'b0;			
				alu_op <= 5'b00000;
				rdst <= 1'b1;
				jp <= 1'b1;
				br <= 1'b0;
				jr <= 1'b1;
				jal <= 1'b0;
				bex <= 1'b0;
				setx <= 1'b0;
				end
			default: begin
				Rwe <= 1'b0;
			    ALUinB <= 1'b0;
				DMwe <= 1'b0;
				Rwd <= 1'b0;			
				alu_op <= 5'b00000;
				rdst <= 1'b0;
				jp <= 1'b0;
				jal <= 1'b0;
				jr <= 1'b0;
				br <= 1'b0;
				bex <= 1'b0;
				setx <= 1'b0;
			end
		endcase
	end
endmodule
				