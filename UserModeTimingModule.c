//
// Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
// Unauthorized copying of this file, via any medium is strictly prohibited.
// Proprietary and confidential.
// Written by Steve Tickle <Steve@BlockWorks.co>, November 2015.
//


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/workqueue.h>
#include <linux/time.h>
#include <asm/mutex.h>
#include <linux/cpu.h> 




//
//
//
void EnablePMUAccessFromUserSpace(void* info)
{
    /* Enable user-mode access to counters. */
    asm volatile("mcr p15, 0, %0, c9, c14, 0" :: "r"(1));
    /* Program PMU and enable all counters */
    asm volatile("mcr p15, 0, %0, c9, c12, 0" :: "r"(1|16));
    asm volatile("mcr p15, 0, %0, c9, c12, 1" :: "r"(0x8000000f));
}


//
//
//
void DisablePMUAccessFromUserSpace(void* info)
{
    /* Enable user-mode access to counters. */
    asm volatile("mcr p15, 0, %0, c9, c14, 0" :: "r"(1));
    /* Program PMU and enable all counters */
    asm volatile("mcr p15, 0, %0, c9, c12, 0" :: "r"(1|16));
    asm volatile("mcr p15, 0, %0, c9, c12, 1" :: "r"(0x0000000e));
}





//
// Make PMU accessible from user space when module is loaded
//
static int __init ModuleInit(void)
{
    uint32_t    online      = num_online_cpus();
    uint32_t    possible    = num_possible_cpus();
    uint32_t    present     = num_present_cpus();

    printk (KERN_INFO "Online Cpus   : %d\n",     online   );
    printk (KERN_INFO "Possible Cpus : %d\n",     possible );
    printk (KERN_INFO "Present Cpus  : %d\n",     present  );

    on_each_cpu(EnablePMUAccessFromUserSpace, NULL, 1);

    printk ("User mode cycle counter access enabled.\n");

    return 0;
}

//
// Disable PMU access from user space when module is unloaded
//
static void __exit ModuleExit(void)
{
    //on_each_cpu(DisablePMUAccessFromUserSpace, NULL, 1);
    on_each_cpu(DisablePMUAccessFromUserSpace, NULL, 1);

    printk ("User mode cycle counter access disabled.\n");
}



 
module_init(ModuleInit);
module_exit(ModuleExit);


MODULE_LICENSE("BlockWorks");
MODULE_AUTHOR("Steve Tickle");
MODULE_DESCRIPTION("Enable access to cycle counter from user mode.");
