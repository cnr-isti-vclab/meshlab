/*********************************************************************/
/*                                                                   */
/*             Optimized BLAS libraries                              */
/*                     By Kazushige Goto <kgoto@tacc.utexas.edu>     */
/*                                                                   */
/* Copyright (c) The University of Texas, 2009. All rights reserved. */
/* UNIVERSITY EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES CONCERNING  */
/* THIS SOFTWARE AND DOCUMENTATION, INCLUDING ANY WARRANTIES OF      */
/* MERCHANTABILITY, FITNESS FOR ANY PARTICULAR PURPOSE,              */
/* NON-INFRINGEMENT AND WARRANTIES OF PERFORMANCE, AND ANY WARRANTY  */
/* THAT MIGHT OTHERWISE ARISE FROM COURSE OF DEALING OR USAGE OF     */
/* TRADE. NO WARRANTY IS EITHER EXPRESS OR IMPLIED WITH RESPECT TO   */
/* THE USE OF THE SOFTWARE OR DOCUMENTATION.                         */
/* Under no circumstances shall University be liable for incidental, */
/* special, indirect, direct or consequential damages or loss of     */
/* profits, interruption of business, or related expenses which may  */
/* arise from use of Software or Documentation, including but not    */
/* limited to those resulting from defects in Software and/or        */
/* Documentation, or loss or inaccuracy of data of any kind.         */
/*********************************************************************/

#include <linux/version.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <asm/pgtable.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/spinlock.h>
#include <linux/mm.h>
#ifdef CONFIG_BIGPHYS_AREA
#include <linux/bigphysarea.h>
#endif
#include <asm/current.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <asm/io.h>

typedef struct {
  pid_t	  pid;
#ifndef CONFIG_BIGPHYS_AREA
  long size;
#endif
  caddr_t address;

} buffer_t;

#define MAX_BUFF_SIZE 1024
#define MAX_LENGTH	(4UL << 20)

static spinlock_t lock __attribute__((aligned(64)));

static buffer_t buffer[MAX_BUFF_SIZE];

static       dev_t mapper_dev;
static struct cdev mapper_cdev;

static int mapper_open   (struct inode *inode, struct file *fp){ return 0;}

static int mapper_release(struct inode *inode, struct file *fp){

  int pos;
#ifndef CONFIG_BIGPHYS_AREA
  caddr_t addr;
#endif

  // printk("Releasing memory...  %d\n", current -> tgid);

  spin_lock(&lock);

  for (pos = 0; pos < MAX_BUFF_SIZE; pos ++) {
    if (buffer[pos].pid == (pid_t) current -> tgid) {

#ifdef CONFIG_BIGPHYS_AREA
      bigphysarea_free_pages(buffer[pos].address);
#else
      
      for (addr = buffer[pos].address; addr < buffer[pos].address + buffer[pos].size; addr += PAGE_SIZE) {
      	ClearPageReserved(virt_to_page(addr));
      }

      kfree(buffer[pos].address);
      buffer[pos].size = 0;
#endif
      buffer[pos].pid = 0;
      buffer[pos].address = 0;
    }
  }

  spin_unlock(&lock);

  return 0;
}

int mapper_mapper(struct file *fp, struct vm_area_struct *vma){

  int ret, pos;
  caddr_t alloc_addr;
#ifndef CONFIG_BIGPHYS_AREA
  caddr_t addr;
#endif
  long all_length, length, current_addr;

  all_length   = vma->vm_end - vma->vm_start;
  current_addr = vma -> vm_start;
  
  spin_lock(&lock);

  while (all_length > 0) {
    length = all_length;
    if (length > MAX_LENGTH) length = MAX_LENGTH;
    all_length -= MAX_LENGTH;

    // printk("Allocating memory...  %d\n", length);

    pos = 0;
    while ((pos < MAX_BUFF_SIZE) && (buffer[pos].address != 0)) pos ++;
    
    if (pos >= MAX_BUFF_SIZE) {
      
      printk("Memory Allocator : too much memory allocation requested.\n");

      spin_unlock(&lock);
      
      return -EIO;
    }
    
#ifdef CONFIG_BIGPHYS_AREA
    alloc_addr = (caddr_t)bigphysarea_alloc_pages(length >> PAGE_SHIFT, 1, GFP_KERNEL);
#else
    alloc_addr = (caddr_t)kmalloc(length, GFP_KERNEL);
#endif
    
    if (alloc_addr == (caddr_t)NULL) {
      
      spin_unlock(&lock);
      
      return -EIO;
    }
    
#ifndef CONFIG_BIGPHYS_AREA
    for (addr = alloc_addr; addr < alloc_addr + length; addr += PAGE_SIZE) {
      clear_page(addr);
      SetPageReserved(virt_to_page(addr));
    }
#endif
    
    if ((ret = remap_pfn_range(vma,
			       current_addr,
			       virt_to_phys((void *)alloc_addr) >> PAGE_SHIFT,
			     length,
			       PAGE_SHARED)) < 0) {
      
#ifdef CONFIG_BIGPHYS_AREA
      bigphysarea_free_pages((caddr_t)alloc_addr);
#else
      
      for (addr = alloc_addr; addr < alloc_addr + length; addr += PAGE_SIZE)  ClearPageReserved(virt_to_page(addr));
      
      kfree((caddr_t)alloc_addr);
#endif
      
      spin_unlock(&lock);
      
      return ret;
    }
    
    buffer[pos].pid = current -> tgid;
    buffer[pos].address = alloc_addr;
#ifndef CONFIG_BIGPHYS_AREA
    buffer[pos].size    = length;
#endif

    current_addr += length;
  }

  spin_unlock(&lock);

  return 0;
}

static struct file_operations mapper_fops = {
        .open    = mapper_open,
        .release = mapper_release,
        .mmap    = mapper_mapper,
        .owner   = THIS_MODULE,
};

static int __init mapper_init(void){

  int ret, i;

  ret = alloc_chrdev_region(&mapper_dev, 0, 1, "mapper");
  
  cdev_init(&mapper_cdev, &mapper_fops);

  ret = cdev_add(&mapper_cdev, mapper_dev, 1);
  
  spin_lock_init(&lock);

  for (i = 0; i < MAX_BUFF_SIZE; i++) {
    buffer[i].pid = 0;
#ifndef CONFIG_BIGPHYS_AREA
    buffer[i].size = 0;
#endif
    buffer[i].address = 0;
  }

  return ret;
}

static void __exit mapper_exit(void){

  int pos;

  for (pos = 0; pos < MAX_BUFF_SIZE; pos ++) {
    if (buffer[pos].address != 0) {
#ifdef CONFIG_BIGPHYS_AREA
      bigphysarea_free_pages(buffer[pos].address);
#else
      kfree(buffer[pos].address);
#endif
    }
  }
  
  cdev_del(&mapper_cdev);

  unregister_chrdev_region(mapper_dev, 1);
}

module_init(mapper_init);
module_exit(mapper_exit);
MODULE_DESCRIPTION("BigPhysArea User Mapping Driver");
MODULE_LICENSE("Unknown");
