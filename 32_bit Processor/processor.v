 /**
 * READ THIS DESCRIPTION!
 *
 * The processor takes in several inputs from a skeleton file.
 *
 * Inputs
 * clock: this is the clock for your processor at 50 MHz
 * reset: we should be able to assert a reset to start your pc from 0 (sync or
 * async is fine)
 *
 * Imem: input data from imem
 * Dmem: input data from dmem
 * Regfile: input data from regfile
 *
 * Outputs
 * Imem: output control signals to interface with imem
 * Dmem: output control signals and data to interface with dmem
 * Regfile: output control signals and data to interface with regfile
 *
 * Notes
 *
 * Ultimately, your processor will be tested by subsituting a master skeleton, imem, dmem, so the
 * testbench can see which controls signal you active when. Therefore, there needs to be a way to
 * "inject" imem, dmem, and regfile interfaces from some external controller module. The skeleton
 * file acts as a small wrapper around your processor for this purpose.
 *
 * You will need to figure out how to instantiate two memory elements, called
 * "syncram," in Quartus: one for imem and one for dmem. Each should take in a
 * 12-bit address and allow for storing a 32-bit value at each address. Each
 * should have a single clock.
 *
 * Each memory element should have a corresponding .mif file that initializes
 * the memory element to certain value on start up. These should be named
 * imem.mif and dmem.mif respectively.
 *
 * Importantly, these .mif files should be placed at the top level, i.e. there
 * should be an imem.mif and a dmem.mif at the same level as process.v. You
 * should figure out how to point your generated imem.v and dmem.v files at
 * these MIF files.
 *
 * imem
 * Inputs:  12-bit address, 1-bit clock enable, and a clock
 * Outputs: 32-bit instruction
 *
 * dmem
 * Inputs:  12-bit address, 1-bit clock, 32-bit data, 1-bit write enable
 * Outputs: 32-bit data at the given address
 *
 */
module processor(
    // Control signals
    clock,                          // I: The master clock
    reset,                          // I: A reset signal

    // Imem
    address_imem,                   // O: The address of the data to get from imem
    q_imem,                         // I: The data from imem

    // Dmem
    address_dmem,                   // O: The address of the data to get or put from/to dmem
    data,                           // O: The data to write to dmem
    wren,                           // O: Write enable for dmem
    q_dmem,                         // I: The data from dmem

    // Regfile
    ctrl_writeEnable,               // O: Write enable for regfile
    ctrl_writeReg,                  // O: Register to write to in regfile
    ctrl_readRegA,                  // O: Register to read from port A of regfile
    ctrl_readRegB,                  // O: Register to read from port B of regfile
    data_writeReg,                  // O: Data to write to for regfile
    data_readRegA,                  // I: Data from port A of regfile
    data_readRegB                  // I: Data from port B of regfile
	 
);
    // Control signals
    input clock, reset;

    // Imem
    output [11:0] address_imem;
    input [31:0] q_imem;

    // Dmem
    output [11:0] address_dmem;
    output [31:0] data;
    output wren;
    input [31:0] q_dmem;

    // Regfile
    output ctrl_writeEnable;
    output [4:0] ctrl_writeReg, ctrl_readRegA, ctrl_readRegB;
    output [31:0] data_writeReg;
    input [31:0] data_readRegA, data_readRegB;

    /* YOUR CODE STARTS HERE */

//------------------------------------------------------------------
    // PC and PC+1
    // Instantiate PC
    wire [31:0] pc_out, pc_in;
    wire enable;
    assign enable = 1'b1;
    register pc(pc_out, pc_in, clock, enable, reset);

    // Instantiate adder(PC+4)
    wire [31:0] adder_in, adder_out;
    add_four add4(adder_out, adder_in);
	 
	  // Connect PC to adder(PC+4)
    assign adder_in = pc_out;
    //assign pc_in = adder_out;

//------------------------------------------------------------------
    // ALU

    // ALU input
    wire [31:0] ALU_operandA, ALU_operandB;
    wire [4:0] ALU_opcode, ALU_shiftamt;

    // ALU output
    wire [31:0] ALU_output;
    wire neq, lt, of;

    alu my_alu(ALU_operandA, ALU_operandB, ALU_opcode, ALU_shiftamt, ALU_output, neq, lt, of);

//------------------------------------------------------------------
    // Instantiate control module
    wire [4:0] ctrl_instOpcodeIn;
    wire [4:0] ctrl_ALUOpcodeIn;
    wire [4:0] ctrl_ALUOpcodeOut;
    wire Rwe, ALUinB, DMwe, Rwd, rdst;
    wire br, jp, jal, jr, bex, setx;
    ctrl_code ctrl_module(ctrl_instOpcodeIn, ctrl_ALUOpcodeIn, neq, lt, Rwe, ALUinB, DMwe, Rwd, rdst, ctrl_ALUOpcodeOut, jp, br, jal, jr, bex, setx);

//------------------------------------------------------------------
    // also connect to module output for debugging
    assign address_imem[11:0] = pc_out[11:0];

    // Read instruction from Imem, q_imem.
    wire [4:0] inst_opcode, rd, rs, rt, shamt, alu_op;
    wire [16:0] immediate;
    wire [31:0] target;

    assign inst_opcode[4:0] = q_imem[31:27];
    assign rd[4:0] = q_imem[26:22];
    assign rs[4:0] = q_imem[21:17];
    assign rt[4:0] = q_imem[16:12];
    assign shamt[4:0] = q_imem[11:7];
    assign alu_op[4:0] = q_imem[6:2];
    assign immediate[16:0] = q_imem[16:0];
    assign target[26:0] = q_imem[26:0];
    assign target[31:27] = 5'b0;

//------------------------------------------------------------------
    // now connect to control module
    assign ctrl_instOpcodeIn[4:0] = inst_opcode[4:0];
    assign ctrl_ALUOpcodeIn[4:0] = alu_op[4:0];

    // connect dmem enable to module output
    assign wren = DMwe;

    // connect to regfile, also connect Rwe to module output
    assign ctrl_writeEnable = Rwe;

//------------------------------------------------------------------
	 //overflow detection
    wire [4:0] writeback_dest;
    wire [31:0] handler_data_out;
    exception error_handler(handler_data_out, writeback_dest, of, rd, ALU_output, inst_opcode, alu_op);
  
    wire [4:0] temp_rd; // accomodate for exception handling
    assign temp_rd = setx ? 5'd30 : writeback_dest;
    assign ctrl_writeReg = jal ? 5'd31 : temp_rd;

//------------------------------------------------------------------
	 //connect the block above together
	 
    assign ctrl_readRegA = jp ? 5'b0 : rs;

    // add a mux for rdestB to select between rd and rt to accomodate for sw
    wire [4:0] temp_ctrl_readRegB;
    assign temp_ctrl_readRegB = rdst ? rd : rt;
    assign ctrl_readRegB = bex ? 5'd30 : temp_ctrl_readRegB;

    // we might write the value from register into dmem, so connect to output wire "data"
    assign data = data_readRegB;

    // connect regfile to ALU
    // first, need to sign extend the immediate value
    wire [31:0] imm_val;
    assign imm_val[31:17] = immediate[16] ? 15'b111111111111111 : 15'b0;
    assign imm_val[16:0] = immediate[16:0];

    // accomodate for branch
    wire [31:0] branch_res;
    branch_adder b_adder(adder_out, imm_val, branch_res);

    // connect the first value read from regfile to first operand of ALU
    assign ALU_operandA = data_readRegA;

    // now use a mux to connect either regfile_readResultB or imm_val to second operand of ALU
    assign ALU_operandB = ALUinB ? imm_val : data_readRegB;
    assign ALU_opcode = ctrl_ALUOpcodeOut;
    assign ALU_shiftamt = shamt;

    // connect ALU output to address_dmem for lw operations
    assign address_dmem = ALU_output[11:0];

    // select which value to write back to regfile
    // accomodate for rstatus
    wire [31:0] temp_data_writeReg;
    assign temp_data_writeReg = Rwd ? q_dmem : handler_data_out;
    assign data_writeReg = jal ? adder_out : temp_data_writeReg;

    wire [31:0] br_normal_pc, jp_br_pc;
    // wire is_branch;
    // or branch_flag(is_branch, neq );
    // wire branch_flag;
    // branch_decider(inst_opcode, neq, lt, branch_flag);
    assign br_normal_pc = br ? branch_res : adder_out;
    assign jp_br_pc = jp ? target : br_normal_pc;
    assign pc_in = jr ? data_writeReg : jp_br_pc;
endmodule