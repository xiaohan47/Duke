# Project Checkpoint 4		
## Simple Processor
***
## Team
**Bogong Yang** netID: by73  
**Xiao Han** netID: xh114
## Description 
### General Idea ###
For this checkpoint, we first declare the main components and wires for them in processor.v, such as the program counter, ALU, datapath control module, and the exception handler. Based on our understanding of the control flow of the datapath, we then connect these "larger" components together using wires and muxes when necessary.

For testing, we created our own skeleton and wired out the inputs and outputs of the processor module, then we used waveform to test R type instructions and I type instructions separately. We then run testcases that have both types of instructions. During the testing process, we realized that we needed to divide the clock so that our design accomodates for the slowest component (Dmem). 

## Main components
### 1.PC		
At every clock cycle, it will pass the address of the next line of MIPS code to i_mem. It will also be incremented by 1 per cycle. Notice that we increase PC by 1 instead of 4 since the mif format is addressed by continuous addresses.  

### 2.Read instruction from Imem
Assign the `opcode`,`rd`,`rs`,`shift_amount`,`alu_opcode`,and `immediate` fields from the 32 bits instruction coming out from Imem.
### 3.Datapath Control Module `ctrl_code.v`
Takes the `opcode` and `alu_opcode` as the inputs, and output the control signals `Rwe`,`ALU in B`,`DMwe`,`Rwd`,`rdst`and `ALU_opcode_out` depending on the type of instruction. Because the ALU_opcode will be 5b'0 when the instruction is I type, we also need to assign ALU opcode accordingly.
### 4.Register File		
Connect the `rs` to `ctrl_readRegA`. Also decide whether to choose `rd` or `rt` for the second read port of regfile based on `rdst`). The register file writes back data from the output of Dmem or Alu_out or exception code into registers -- We will discuss error handling part later. The `Rwe` will control the enable of writing back to register file.	
### 5.ALU		
The output from `data_readRegA` from `register file` is directly connected to `ALU_operandA`. Either `immediate`(The sign extended immediate value from instruction) or `data_readRegB` are passed to the ALU as the second operand based on the `ALUinB` control signal. The ALU will perform the function specified by `ALU_opcode` which we get from the `control Model`;		
### 6.Writing back to Register 		
The output from either Dmem or ALU is written back to the `register file`, and the write is only successful when `Rwe` is enabled. The `ALU_output` here accomodates for the overflow situation, which we will discuss in the next section.
		
### 7.Overflow		
For this checkpoint, we only handle exceptions when there is overflow when executing `add`, `sub`, or `addi`. When we detect an overflow, we need to write the corresponding error code (1,2 or 3) into the rstatus register, which is r30. So we built an exception module, which takes `overflow`,`rd`,`alu_output`,`opcode`,`alu_opcode`into the module so that if there is an overflow, depending on the `opcode`and `alu_opcode`, we assign the corresponding error code to ALU's output and change the write destination for the regfile to r30. If there is no overflow, the `result`will still be the original `alu_output`. 

### 8.Clock
We noticed that the fastest component in our design is the datapath(processor) and the slowest parts are the memory modules, whereas the regfiles lies somewhere in between. Thus, in order to make the fastest component to be able to accomodate for the slowest components, we appied the slowest clock to the processor clock (original clock frequency / 4), second slowest clock (half of the original clock frequency) to regfile, and the fastest clock (original clock) to both memory modules.

## References:
We looked up designs and code from [this website](http://referencedesigner.com/tutorials/verilogexamples/verilog_ex_02.php) when implementing both of our clock frequency dividers.