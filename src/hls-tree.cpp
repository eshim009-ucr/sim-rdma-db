#include "hls-tree.hpp"

void hls_search(Tree& tree, hls::stream<bkey_t> &input,
    hls::stream<bval_t> &output,
    hls::stream<int> &search2mem,
    hls::stream<Node> &mem2search
) {
    static enum {
        INIT,
        READ,
        RESP
    } state = INIT;
    static bkey_t key;
    static bval_t value;


    switch(state) {
        case INIT:
            if (!input.empty()) {
                state = READ;
            }
            break;
        case READ:
            input.read(key);
            state = RESP;
            break;
        case RESP:
            if (!output.full()) {
                search(&tree, key, &value);
                output.write(value);
                state = INIT;
            }
            break;
    }
}
