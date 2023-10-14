#include <stdio.h>

void func_b(void)
{
    printk("%s in %s\n", __func__ ,__FILE__);
}