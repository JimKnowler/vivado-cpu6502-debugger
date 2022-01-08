set_property SRC_FILE_INFO {cfile:c:/Users/jim/Documents/GitHub/personal/vivado/vivado-cpu6502-debugger/vivado-debugger.srcs/sources_1/ip/clk_wiz_0/clk_wiz_0/clk_wiz_0_in_context.xdc rfile:../../../vivado-debugger.srcs/sources_1/ip/clk_wiz_0/clk_wiz_0/clk_wiz_0_in_context.xdc id:1 order:EARLY scoped_inst:pll} [current_design]
set_property SRC_FILE_INFO {cfile:C:/Users/jim/Documents/GitHub/personal/vivado/vivado-cpu6502-debugger/vivado-debugger.srcs/constrs_1/imports/digilent-xdc/Arty-A7-100-Master.xdc rfile:../../../vivado-debugger.srcs/constrs_1/imports/digilent-xdc/Arty-A7-100-Master.xdc id:2} [current_design]
current_instance pll
set_property src_info {type:SCOPED_XDC file:1 line:1 export:INPUT save:INPUT read:READ} [current_design]
create_clock -period 10.000 [get_ports -no_traverse {}]
set_property src_info {type:SCOPED_XDC file:1 line:4 export:INPUT save:INPUT read:READ} [current_design]
create_generated_clock -source [get_ports i_clk_100mhz] -edges {1 2 3} -edge_shift {0.000 95.000 190.000} [get_ports {}]
current_instance
set_property src_info {type:XDC file:2 line:7 export:INPUT save:INPUT read:READ} [current_design]
set_property -dict { PACKAGE_PIN E3    IOSTANDARD LVCMOS33 } [get_ports { i_clk_100mhz }]; #IO_L12P_T1_MRCC_35 Sch=gclk[100]
set_property src_info {type:XDC file:2 line:87 export:INPUT save:INPUT read:READ} [current_design]
set_property -dict { PACKAGE_PIN V15   IOSTANDARD LVCMOS33 } [get_ports { i_spi_cs_n  }]; #IO_L16P_T2_CSI_B_14 Sch=ck_io[0]
set_property src_info {type:XDC file:2 line:165 export:INPUT save:INPUT read:READ} [current_design]
set_property -dict { PACKAGE_PIN G1    IOSTANDARD LVCMOS33 } [get_ports { o_spi_cipo }]; #IO_L17N_T2_35 Sch=ck_miso
set_property src_info {type:XDC file:2 line:166 export:INPUT save:INPUT read:READ} [current_design]
set_property -dict { PACKAGE_PIN H1    IOSTANDARD LVCMOS33 } [get_ports { i_spi_copi }]; #IO_L17P_T2_35 Sch=ck_mosi
set_property src_info {type:XDC file:2 line:167 export:INPUT save:INPUT read:READ} [current_design]
set_property -dict { PACKAGE_PIN F1    IOSTANDARD LVCMOS33 } [get_ports { i_spi_clk }]; #IO_L18P_T2_35 Sch=ck_sck
set_property src_info {type:XDC file:2 line:171 export:INPUT save:INPUT read:READ} [current_design]
set_property CLOCK_DEDICATED_ROUTE FALSE [get_nets i_spi_clk_IBUF];
set_property src_info {type:XDC file:2 line:181 export:INPUT save:INPUT read:READ} [current_design]
set_property -dict { PACKAGE_PIN C2    IOSTANDARD LVCMOS33 } [get_ports { i_reset_n }]; #IO_L16P_T2_35 Sch=ck_rst
