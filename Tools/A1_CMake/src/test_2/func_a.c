#include <stdio.h>

void func_a(void)
{
   printk("%s in %s\n", __func__ ,__FILE__);
}