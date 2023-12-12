module simple_module(input clk, input reset, input enable, output reg [7:0] counter);

always @(posedge clk) begin
    if (reset)
        counter <= 0;
    else if (enable)
        counter <= counter + 1;
end

endmodule
