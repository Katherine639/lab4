//lab 4 changes
int shm_open(int id, char **pointer) {
  int i = 0;
  int j = 0;
  char *mem; 
  acquire(&(shm_table.lock));
  struct proc *curproc = myproc();

//CAse 1 ->exist
  for(i = 0; i < 64; i++) {
    if(shm_table.shm_pages[i].id == id) {
	  //mappages(pde_t *pgdir, void *va, uint size, uint pa, int perm);
      mappages(curproc->pgdir,(void *)PGROUNDUP(curproc->sz), PGSIZE, V2P(shm_table.shm_pages[i].frame), PTE_W|PTE_U);
      shm_table.shm_pages[i].refcnt = shm_table.shm_pages[i].refcnt + 1;
      *pointer = (char *)PGROUNDUP(curproc->sz);
      curproc->sz += PGSIZE;
      release(&(shm_table.lock));    
      return 0; 
     }
   }
 //Case 2
  for(j = 0; j < 64; j++) {
    if(shm_table.shm_pages[j].id == 0 && shm_table.shm_pages[j].refcnt == 0) {
      shm_table.shm_pages[j].id = id;
      mem = kalloc(); 
      memset(mem, 0, PGSIZE); 
      shm_table.shm_pages[j].frame = mem; 
      shm_table.shm_pages[j].refcnt = 1;
      mappages(curproc->pgdir,(void *)PGROUNDUP(curproc->sz), PGSIZE, V2P(shm_table.shm_pages[j].frame), PTE_W|PTE_U);
      *pointer = (char *)PGROUNDUP(curproc->sz);
      //curproc->sz += PGSIZE; 
      release(&(shm_table.lock));  
      return 0;  
    }
  }
  
  return 1;
}
int shm_close(int id) {
  int i = 0;
  acquire(&(shm_table.lock)); 
  for(i = 0; i < 64; i++) {
    if(shm_table.shm_pages[i].id == id) {
	//check for last one then clear memory -> so reference count has to be 1 first
      shm_table.shm_pages[i].refcnt = shm_table.shm_pages[i].refcnt - 1;
      if(shm_table.shm_pages[i].refcnt == 0) {
        shm_table.shm_pages[i].frame = 0;
        shm_table.shm_pages[i].id = 0;
      }
    } 
  }
  release(&(shm_table.lock));
  return 0;
