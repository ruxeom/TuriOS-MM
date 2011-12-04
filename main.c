#include <stdio.h>
#include "mm.h"
#include "pc.h"

extern pc_t pc; /* pc.h */
extern mm_t mm; /* mm.h */


/* Entry point to test program */
int main(void)
{
    int i;
    ptab_entry_t *ptab;

    /* Not so magical emulation */
    char *mbase = (char *)(
                            (unsigned int)&pc.mem[2 * MM_PAGE_SIZE]
                            & ~(MM_PAGE_SIZE - 1)
                            );

    mm.pfbm = (bitmap_t *)      (mbase +  0 * MM_PAGE_SIZE);
    mm.pdir = (ptab_entry_t *)  (mbase +  1 * MM_PAGE_SIZE);
    ptab = (ptab_entry_t *)     (mbase +  2 * MM_PAGE_SIZE);

    mm.gdt = (gdt_ptr_t *)    (mbase + 10 * MM_PAGE_SIZE);
    /* Emulation ends here */

    gdt_load(mm.gdt);
    bitmap_clear(mm.pfbm);
    for(i = 0; i < 1024; i++) {
        ptab_entry_clear(&mm.pdir[i]);
        ptab_entry_set_bit_p(&mm.pdir[i], 1);
    }
    ptab_entry_set_pfa(mm.pdir, (uint32_t)ptab);
    ptab_load(mm.pdir);
    

    puts("vm_request");
    puts("----------");

    do {
        int i;
        struct vm_request_args {
            uint32_t vaddr;
            int length;
            int expected;
            char finish;
        };

        struct vm_request_args vra[] = {
            {0,     1, 1, 1},
            {4096,  1, 1, 1},
            {4096,  1, 0, 1},
            {8192,  3, 1, 1},
            {0,     0, 0, 0},
        };
    
        for(i = 0; 0 < vra[i].finish; i++) {
            printf("vm_request(%d, %d) => %d | %d\n",
                vra[i].vaddr,
                vra[i].length,
                vm_request(vra[i].vaddr, vra[i].length),
                vra[i].expected
            );
        }
    } while(0);

    puts("");
    puts("vm_return");
    puts("----------");

    do {
        int i;
        struct vm_return_args {
            uint32_t vaddr;
            int expected;
            char finish;
        };

        struct vm_return_args vra[] = {
            {0,     1, 1},
            {4096,  1, 1},
            {4096,  0, 1},
            {8192,  1, 1},
            {0,     0, 0},
        };
    
        for(i = 0; 0 < vra[i].finish; i++) {
            printf("vm_return(%d) => %d | %d\n",
                vra[i].vaddr,
                vm_return(vra[i].vaddr),
                vra[i].expected
            );
        }
    } while(0);

    return 0;
}
