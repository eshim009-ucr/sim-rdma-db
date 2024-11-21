#include "krnl.hpp"
#include "hls-tree.hpp"

const int NUM_NODES = 2; 

void rdma_read(
    int s_axi_lqpn,
    ap_uint<64> s_axi_laddr,
    ap_uint<64> s_axi_raddr,
    int s_axi_len,
    hls::stream<pkt256>& m_axis_tx_meta
){
    //#pragma HLS dataflow
    #pragma HLS inline off
    #pragma HLS pipeline II=1
    pkt256 tx_meta;

    /*RDMA OP*/
    tx_meta.data.range(2,0) = 0x00000000; 
    /*lQPN*/
    tx_meta.data.range(26,3) = s_axi_lqpn; 
    /*
    lAddr
    */
    tx_meta.data.range(74, 27) = s_axi_laddr; 
    /*rAddr*/
    tx_meta.data.range(122, 75) = s_axi_raddr; 
    //+(itt*4)
    /*len*/
    tx_meta.data.range(154, 123) = s_axi_len;
    m_axis_tx_meta.write(tx_meta);
    
}

void rdma_write(
    int s_axi_lqpn,
    ap_uint<64> s_axi_laddr,
    ap_uint<64> s_axi_raddr,
    int s_axi_len,
    ap_uint<64>  write_value,
    hls::stream<pkt256>& m_axis_tx_meta, 
    hls::stream<pkt64>& m_axis_tx_data
){
    //#pragma HLS dataflow
    #pragma HLS inline off
    #pragma HLS pipeline II=1
    
    pkt256 tx_meta;
    pkt64 tx_data;

    /*RDMA OP*/
    tx_meta.data.range(2,0) = 0x00000001; 
    /*lQPN*/
    tx_meta.data.range(26,3) = s_axi_lqpn; 
    /*
    lAddr
    if 0 writes from tx_data. 
    */
    tx_meta.data.range(74, 27) = s_axi_laddr; 
    /*rAddr*/
    tx_meta.data.range(122, 75) = s_axi_raddr; 
    //+(itt*4)
    /*len*/
    tx_meta.data.range(154, 123) = s_axi_len;
    
    m_axis_tx_meta.write(tx_meta);

    //Write data only if laddr is 0
    if (s_axi_laddr == 0) {
        tx_data.data(63, 0) = write_value;
        tx_data.keep(7, 0) = 0xff;
        tx_data.last = 1; 
        m_axis_tx_data.write(tx_data);
    }

    
}


void rdma_write_through(
    int s_axi_lqpn,
    ap_uint<64> s_axi_laddr,
    ap_uint<64> s_axi_raddr,
    int s_axi_len,
    ap_uint<64>  write_value,
    hls::stream<pkt256>& m_axis_tx_meta, 
    hls::stream<pkt64>& m_axis_tx_data
){
    //#pragma HLS dataflow
    #pragma HLS inline off
    #pragma HLS pipeline II=1
    #pragma HLS INTERFACE axis port = m_axis_tx_meta
    #pragma HLS INTERFACE axis port = m_axis_tx_data
    
    pkt256 tx_meta;
    pkt64 tx_data;

    /*RDMA OP*/
    tx_meta.data.range(2,0) = 0x00000004; 
    /*lQPN*/
    tx_meta.data.range(26,3) = s_axi_lqpn; 
    /*
    lAddr
    if 0 writes from tx_data. 
    */
    tx_meta.data.range(74, 27) = s_axi_laddr; 
    /*rAddr*/
    tx_meta.data.range(122, 75) = s_axi_raddr; 
    //+(itt*4)
    /*len*/
    tx_meta.data.range(154, 123) = s_axi_len;
    

    m_axis_tx_meta.write(tx_meta);

    //Write data only if laddr is 0
    if (s_axi_laddr == 0) {
        tx_data.data.range(63, 0) = write_value;
        tx_data.keep(7, 0) = 0xff;
        tx_data.last = 1; 
        
        m_axis_tx_data.write(tx_data);
    }

}

void rdma_bram_read(
    int s_axi_lqpn,
    ap_uint<64> s_axi_laddr,
    ap_uint<64> s_axi_raddr,
    int s_axi_len,
    hls::stream<pkt256>& m_axis_tx_meta
){
    //#pragma HLS dataflow
    #pragma HLS inline off
    #pragma HLS pipeline II=1
    pkt256 tx_meta;

    /*RDMA OP*/
    tx_meta.data.range(2,0) = 0x00000002; 
    /*lQPN*/
    tx_meta.data.range(26,3) = s_axi_lqpn; 
    /*
    lAddr
    */
    tx_meta.data.range(74, 27) = s_axi_laddr; 
    /*rAddr*/
    tx_meta.data.range(122, 75) = s_axi_raddr; 
    //+(itt*4)
    /*len*/
    tx_meta.data.range(154, 123) = s_axi_len;
    m_axis_tx_meta.write(tx_meta);
    
}

void rdma_bram_write(
    int s_axi_lqpn,
    ap_uint<64> s_axi_laddr,
    ap_uint<64> s_axi_raddr,
    int s_axi_len,
    ap_uint<64>  write_value,
    hls::stream<pkt256>& m_axis_tx_meta, 
    hls::stream<pkt64>& m_axis_tx_data
){
    //#pragma HLS dataflow
    #pragma HLS inline off
    #pragma HLS pipeline II=1
    #pragma HLS INTERFACE axis port = m_axis_tx_meta
    #pragma HLS INTERFACE axis port = m_axis_tx_data
    
    pkt256 tx_meta;
    pkt64 tx_data;

    /*RDMA OP*/
    tx_meta.data.range(2,0) = 0x00000003; 
    /*lQPN*/
    tx_meta.data.range(26,3) = s_axi_lqpn; 
    /*
    lAddr
    if 0 writes from tx_data. 
    */
    tx_meta.data.range(74, 27) = s_axi_laddr; 
    /*rAddr*/
    tx_meta.data.range(122, 75) = s_axi_raddr; 
    //+(itt*4)
    /*len*/
    tx_meta.data.range(154, 123) = s_axi_len;
    

    m_axis_tx_meta.write(tx_meta);

    //Write data only if laddr is 0
    if (s_axi_laddr == 0) {
        tx_data.data.range(63, 0) = write_value;
        tx_data.keep(7, 0) = 0xff;
        tx_data.last = 1; 
        
        m_axis_tx_data.write(tx_data);
    }

}


void test_krnl(
    //Outgoing RDMA
    hls::stream<pkt256>& m_axis_tx_meta, 
    hls::stream<pkt64>& m_axis_tx_data,
    hls::stream<pkt64>& s_axis_tx_status,

    //Local BRAM
    hls::stream<pkt256>& m_axis_bram_write_cmd, 
    hls::stream<pkt256>& m_axis_bram_read_cmd,
    hls::stream<pkt512>& m_axis_bram_write_data,
    hls::stream<pkt512>& s_axis_bram_read_data,

    //Incoming
    hls::stream<pkt64>& s_axis_update,

    //Book keeping
    int myBoardNum,

    int RDMA_TYPE,
    int exec,

    //Local HBM
    int *network_ptr
) {

    #pragma HLS INTERFACE axis port = m_axis_tx_meta
    #pragma HLS INTERFACE axis port = m_axis_tx_data
    #pragma HLS INTERFACE axis port = s_axis_tx_status

    #pragma HLS INTERFACE axis port = m_axis_bram_write_cmd
    #pragma HLS INTERFACE axis port = m_axis_bram_read_cmd
    #pragma HLS INTERFACE axis port = m_axis_bram_write_data
    #pragma HLS INTERFACE axis port = s_axis_bram_read_data

    #pragma HLS INTERFACE axis port = s_axis_update

    pkt64 status;
    if (!s_axis_tx_status.empty()) {
        s_axis_tx_status.read(status);
    }

    #pragma HLS DATAFLOW


    //TODO: Your kernel here
    static hls::stream<bkey_t> searchInput;
    static hls::stream<bval_t> searchOutput;
    static hls::stream<int> search2mem;
    static hls::stream<Node> mem2search;
    bval_t searchResult;

    Tree tree;
    init_tree(&tree);
    Node *root = &tree.memory[tree.root];
    bval_t result;

    root->keys[0] = 1; root->values[0].data = 10;
    root->keys[1] = 2; root->values[1].data = 20;
    root->keys[2] = 4; root->values[2].data = 40;
    root->keys[3] = 5; root->values[3].data = 50;

    searchInput.write(0);
    searchInput.write(3);
    searchInput.write(6);
    searchInput.write(1);
    searchInput.write(2);
    searchInput.write(4);
    searchInput.write(5);


    uint_fast32_t opsCount = searchInput.size();
    uint_fast32_t opsIn = 0;
    uint_fast32_t opsOut = 0;

    while (opsOut < opsCount) {
        hls_search(
            tree,
            searchInput,
            searchOutput,
            search2mem,
            mem2search
        );

        while (!searchOutput.empty()) {
            searchOutput.read(searchResult);
            std::cout << "Search Result: ";
            if (searchResult.data == INVALID) {
                std::cout << "Invalid" << std::endl;
            } else {
                std::cout << searchResult.data << "\t0x" << std::hex
                          << searchResult.data << std::dec << std::endl;
            }
            opsOut++;
        }
    }

    return;
}
