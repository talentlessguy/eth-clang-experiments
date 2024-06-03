#include <stdio.h>
#include <stdlib.h>
#include <ethc/account.h>

int main() {
    // Example hex string
    const char *hexstr = getenv("PK"); // 0x9ef...
    
    struct eth_account acc;

    set_pk(hexstr, &acc); // how would this function look like?

    char *addr;
    eth_account_address_get(addr, &acc);
    
    return 0;
}
