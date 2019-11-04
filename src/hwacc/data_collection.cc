#include "data_collection.hh"

void
Results::print() {

    std::cout << "********************************************************************************" << std::endl;
    std::cout << "   ========= Performance Analysis =============" << std::endl;
    std::cout << "   System Clock:                    " << 1.0/(clock_period/1000) << "GHz" << std::endl;
    std::cout << "   Transistor Latency:              " << fu_clock_period << "ns" << std::endl;
    std::cout << "   Runtime:                         " << cycles << " cycles" << std::endl;
    std::cout << "   Runtime:                         " << runtime << " seconds" << std::endl;
    std::cout << "   Stalls:                          " << stalls << " cycles" << std::endl;
    std::cout << "       Load Only:                   " << loadOnlyStall << " cycles" << std::endl;
    std::cout << "       Store Only:                  " << storeOnlyStall << " cycles" << std::endl;
    std::cout << "       Compute Only:                " << compOnlyStall << " cycles" << std::endl;
    std::cout << "       Compute & Store:             " << storeCompStall << " cycles" << std::endl;
    std::cout << "       Load & Store:                " << loadStoreStall << " cycles" << std::endl;
    std::cout << "       Load & Compute:              " << loadCompStall << " cycles" << std::endl;
    std::cout << "       Load & Compute & Store:      " << loadStoreCompStall << " cycles" << std::endl;
    std::cout << "   Executed Nodes:                  " << nodes << " cycles" << std::endl;
    std::cout << "       Load Only:                   " << loadOnly << " cycles" << std::endl;
    std::cout << "       Store Only:                  " << storeOnly << " cycles" << std::endl;
    std::cout << "       Compute Only:                " << compOnly << " cycles" << std::endl;
    std::cout << "       Compute & Store:             " << storeComp << " cycles" << std::endl;
    std::cout << "       Load & Store:                " << loadStore << " cycles" << std::endl;
    std::cout << "       Load & Compute:              " << loadComp << " cycles" << std::endl;
    std::cout << "       Load & Compute & Store:      " << loadStoreComp << " cycles" << std::endl;
    std::cout << std::endl;
    std::cout << "   ========= Memory Configuration =============" << std::endl;
    std::cout << "   Shared Cache Size:               " << cache_size/1024 << "kB" << std::endl;
    std::cout << "   Shared Cache Read Ports:         " << cache_ports << std::endl;
    std::cout << "   Shared Cache Write Ports:        " << cache_ports << std::endl;
    std::cout << "   Local Bus Ports:                 " << local_ports << std::endl;
    std::cout << "   Shared Cache Size:               " << cache_size/1024 << "kB" << std::endl;
    std::cout << "   Private SPM Size:                " << spm_size/1024 << "kB" << std::endl;
    std::cout << "   Private Read Ports:              " << read_ports << std::endl;
    std::cout << "   Private Write Ports:             " << write_ports << std::endl;
    std::cout << "   Private Read Bus Width:          " << read_bus_width << std::endl;
    std::cout << "   Private Write Bus Width:         " << write_bus_width << std::endl;
    std::cout << "       SPM Reads:                   " << mem_reads << std::endl;
    std::cout << "       SPM Writes:                  " << mem_writes << std::endl;
    std::cout << std::endl;
    std::cout << "   ========= Runtime Functional Units =========" << std::endl;
    std::cout << "   Counter FU's:                    " << run_counter << std::endl;
    std::cout << "   Integer Add/Sub FU's:            " << run_add_sub << std::endl;
    std::cout << "   Integer Mul/Div FU's:            " << run_mul_div << std::endl;
    std::cout << "   Integer Shifter FU's:            " << run_shift << std::endl;
    std::cout << "   Integer Bitwise FU's:            " << run_bit << std::endl;
    std::cout << "   Floating Point Float Add/Sub:    " << run_float_add_sub << std::endl;
    std::cout << "   Floating Point Double Add/Sub:   " << run_doub_add_sub << std::endl;
    std::cout << "   Floating Point Float Mul/Div:    " << run_float_mul_div << std::endl;
    std::cout << "   Floating Point Double Mul/Div:   " << run_doub_mul_div << std::endl;
    std::cout << "   0 Cycle Compare FU's:            " << run_zero << std::endl;
    std::cout << "   GEP Instruction FU's:            " << run_gep << std::endl;
    std::cout << "   Type Conversion FU's:            " << run_conv << std::endl;
    std::cout << std::endl;
    std::cout << "   ========= Static Functional Units ========="  << std::endl;
    std::cout << "   Counter FU's:                    " << static_counter << std::endl;
    std::cout << "   Integer Add/Sub FU's:            " << static_add_sub << std::endl;
    std::cout << "   Integer Mul/Div FU's:            " << static_mul_div << std::endl;
    std::cout << "   Integer Shifter FU's:            " << static_shift << std::endl;
    std::cout << "   Integer Bitwise FU's:            " << static_bit << std::endl;
    std::cout << "   Floating Point Float Add/Sub:    " << static_float_add_sub << std::endl;
    std::cout << "   Floating Point Double Add/Sub:   " << static_doub_add_sub << std::endl;
    std::cout << "   Floating Point Float Mul/Div:    " << static_float_mul_div << std::endl;
    std::cout << "   Floating Point Double Mul/Div:   " << static_doub_mul_div << std::endl;
    std::cout << "   0 Cycle Compare FU's:            " << static_zero << std::endl;
    std::cout << "   GEP Instruction FU's:            " << static_gep << std::endl;
    std::cout << "   Type Conversion FU's:            " << static_conv << std::endl;
    std::cout << "   Other:                           " << static_other << std::endl;
    std::cout << std::endl;
    std::cout << "   ========= Pipeline Register Usage =========" << std::endl;
    std::cout << "   Total Number of Registers:       " << reg_total << std::endl;
    std::cout << "   Max Register Usage Per Cycle:    " << reg_max_usage << std::endl;
    std::cout << "   Avg Register Usage Per Cycle:    " << reg_avg_usage << std::endl;
    std::cout << "   Avg Register Size Per Cycle:     " << reg_avg_size << std::endl;
    std::cout << std::endl;
    std::cout << "   ========= Power Analysis ==================" << std::endl;
    std::cout << "   FU Leakage Power:                " << fu_leakage << " mW " << std::endl;
    std::cout << "   FU Dynamic Power:                " << fu_dynamic << " mW " << std::endl;
    std::cout << "   FU Total Power:                  " << fu_total_power << " mW " << std::endl;
    std::cout << "   Pipeline Register Leakage Power: " << reg_leakage  << " mW " << std::endl;
    std::cout << "   Pipeline Register Dynamic Power: " << reg_dynamic << " mW " << std::endl;
    std::cout << "       Total Register Reads:        " << reg_reads << std::endl;
    std::cout << "       Total Register Writes:       " << reg_writes << std::endl;
    std::cout << "   Pipeline Register Total Power:   " << reg_total_power <<  " mW" << std::endl;
    std::cout << "   SPM Leakage Power:               " << spm_leakage << " mW" << std::endl;
    std::cout << "   SPM Optimized Leakage Power:     " << spm_opt_leakage << " mW" << std::endl;
    std::cout << "   SPM Read Dynamic Power:          " << spm_read_dynamic << " mW" << std::endl;
    std::cout << "   SPM Write Dynamic Power:         " << spm_write_dynamic << " mW" << std::endl;
    std::cout << "   SPM Total Power:                 " << spm_leakage+spm_read_dynamic+spm_write_dynamic << " mW" << std::endl;
    std::cout << "   Total Accelerator Power:         " << total_power+spm_leakage+spm_read_dynamic+spm_write_dynamic << " mW" << std::endl;
    std::cout << std::endl;
    std::cout << "   ========= Area Analysis ===================" << std::endl;
    std::cout << "   FU Area:                         " << fu_area << " um^2 (" << fu_area/1000000 << " mm^2)" << std::endl;
    std::cout << "   SPM Area:                        " << spm_area/1000 << " um^2" << std::endl;
    std::cout << "   SPM Opt Area:                    " << spm_opt_area/1000 << " um^2" << std::endl;
    std::cout << "   Register Area:                   " << reg_area << " um^2 (" << reg_area/1000000 << " mm^2)" << std::endl;
    std::cout << "   Total Accelerator Area:          " << total_area + spm_area/1000 << " um^2 (" << total_area/1000000 << " mm^2)" << std::endl;
    std::cout << std::endl;
    std::cout << "   ========= System Cache Usage ==============" << std::endl;
    std::cout << "   Cache Reads:                     " << dma_reads << std::endl;  //Experimental
    std::cout << "   Cache Writes:                    " << dma_writes << std::endl;  //Experimental
    std::cout << "   Cache Leakage Power:             " << cache_leakage << " mW" << std::endl;
    std::cout << "   Cache Read Dynamic Power:        " << cache_read_dynamic << " mW" << std::endl;
    std::cout << "   Cache Write Dynamic Power:       " << cache_write_dynamic << " mW" << std::endl;
    std::cout << "   Cache Area:                      " << cache_area/1000 << " um^2" << std::endl; 
    std::cout << std::endl;

}










void
Results::simpleStats() {
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "StatsStart:\n";
    std::cout << "\n" << 1.0/(clock_period/1000);
    std::cout << "\n," << fu_clock_period;
    std::cout << "\n," << cycles;
    std::cout << "\n," << runtime;
    std::cout << "\n," << stalls;
    std::cout << "\n," << nodes;
    std::cout << "\n," << cache_size/1024;
    std::cout << "\n," << spm_size/1024;
    std::cout << "\n," << read_ports;
    std::cout << "\n," << write_ports;
    std::cout << "\n," << read_bus_width;
    std::cout << "\n," << write_bus_width;
    std::cout << "\n," << run_counter;
    std::cout << "\n," << run_add_sub;
    std::cout << "\n," << run_mul_div;
    std::cout << "\n," << run_shift;
    std::cout << "\n," << run_bit;
    std::cout << "\n," << run_float_add_sub;
    std::cout << "\n," << run_doub_add_sub;
    std::cout << "\n," << run_float_mul_div;
    std::cout << "\n," << run_doub_mul_div;
    std::cout << "\n," << run_zero;
    std::cout << "\n," << run_gep;
    std::cout << "\n," << run_conv;
    std::cout << "\n," << static_counter;
    std::cout << "\n," << static_add_sub;
    std::cout << "\n," << static_mul_div;
    std::cout << "\n," << static_shift;
    std::cout << "\n," << static_bit;
    std::cout << "\n," << static_float_add_sub;
    std::cout << "\n," << static_doub_add_sub;
    std::cout << "\n," << static_float_mul_div;
    std::cout << "\n," << static_doub_mul_div;
    std::cout << "\n," << static_zero;
    std::cout << "\n," << static_gep;
    std::cout << "\n," << static_conv;
    std::cout << "\n," << static_other;
    std::cout << "\n," << reg_total;
    std::cout << "\n," << reg_max_usage;
    std::cout << "\n," << reg_avg_usage;
    std::cout << "\n," << reg_avg_size;
    std::cout << "\n," << fu_leakage;
    std::cout << "\n," << fu_dynamic;
    std::cout << "\n," << fu_total_power;
    std::cout << "\n," << reg_leakage;
    std::cout << "\n," << reg_dynamic;
    std::cout << "\n," << reg_reads;
    std::cout << "\n," << reg_writes;
    std::cout << "\n," << reg_total_power;
    std::cout << "\n," << spm_leakage;
    std::cout << "\n," << spm_read_dynamic;
    std::cout << "\n," << spm_write_dynamic;
    std::cout << "\n," << spm_leakage+spm_read_dynamic+spm_write_dynamic;
    std::cout << "\n," << total_power+spm_leakage+spm_read_dynamic+spm_write_dynamic;
    std::cout << "\n," << fu_area;
    std::cout << "\n," << spm_area;
    std::cout << "\n," << reg_area;
    std::cout << "\n," << total_area + spm_area/1000;
    std::cout << "\n," << mem_reads;
    std::cout << "\n," << mem_writes;
    std::cout << "\n," << cache_leakage;
    std::cout << "\n," << cache_read_dynamic;
    std::cout << "\n," << cache_write_dynamic;
    std::cout << "\n," << cache_area/1000;
    std::cout << "\n";
}