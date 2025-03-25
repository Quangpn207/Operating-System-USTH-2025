
#include <stdio.h>
#include <string.h>

#define NB_DISK_PAGE 5
#define PAGE_SIZE 20
#define NB_MEM_PAGE 4

#define boolean int
#define true 1
#define false 0

char Disk[NB_DISK_PAGE][PAGE_SIZE];
char Memory[NB_MEM_PAGE][PAGE_SIZE];
int PageTable[NB_DISK_PAGE];
struct {
	boolean free;
	int date;
	int npage;
} MemState[NB_MEM_PAGE];

int Date = 1;
int PageFaultCount = 0;
void init();
char *memory_read(int npage);
int memory_alloc();
void page_fault(int npage);
int lru_select();

void init() {
	int i;

	// initialize Disk
	for (i=0;i<NB_DISK_PAGE;i++) {
		strcpy(Disk[i],"page");
		Disk[i][4] = '0'+i;
		Disk[i][5] = 0;
	}
	/*
	for (i=0;i<NB_DISK_PAGE;i++) {
		printf(Disk[i]);
		printf("\n");
	}
	*/
	// initialize Memory
	for (i=0;i<NB_MEM_PAGE;i++)
		MemState[i].free = true;

	// initialize PageTable
	for (i=0;i<NB_DISK_PAGE;i++)
		PageTable[i] = -1;
}


char *memory_read(int npage) {
	printf("memory_read (%d)\n",npage);
	Date++;
	int mem_page = PageTable[npage];
	if (mem_page == -1) {
		page_fault(npage);
		mem_page = PageTable[npage];
	}
	MemState[mem_page].date = Date;
	return Memory[mem_page];
}
int memory_alloc() {
	for (int i = 0; i < NB_MEM_PAGE; i++) {
		if (MemState[i].free) {
			MemState[i].free = false;
			printf("memory_alloc: %d\n",i);
			return i;
		}
	}
	return -1;
}


void page_fault(int npage) {
	PageFaultCount++;
	printf("page_fault (%d)\n",npage);
	int mem_page = memory_alloc();
	if (mem_page == -1) {
		mem_page = lru_select();
		int old_page = MemState[mem_page].npage;
		PageTable[old_page] = -1;
	}
	strcpy(Memory[mem_page], Disk[npage]);
	PageTable[npage] = mem_page;
	MemState[mem_page].npage = npage;
	MemState[mem_page].date = Date;
	MemState[mem_page].free = false;
}

int lru_select() {
	int oldest = -1;
	for (int i = 0; i < NB_MEM_PAGE; i++) {
		if (!MemState[i].free && (oldest == -1 || MemState[i].date < MemState[oldest].date)) {
			oldest = i;
		}
	}
	printf("lru_select: %d\n",oldest);
	return oldest;
}

int main(int argc, char *argv[]) {
	int i;
	printf("initialization\n");
	init();

	/*
	printf("linear access of all pages\n");
	for (i=0;i<NB_DISK_PAGE;i++)
		printf("read access page %d : %s\n",i,memory_read(i));
	*/

	printf("access pages as in lecture (0,1,2,3,0,1,4,0,1,2,3,4,1,2)\n");
	int serie[14] = {0,1,2,3,0,1,4,0,1,2,3,4,1,2};
	for (i=0;i<14;i++)
		printf("read access page %d : %s\n",serie[i],memory_read(serie[i]));
	printf("completed\n");
	printf("Page fault count : %d\n",PageFaultCount);
	printf("print memory_state\n");
	for (i=0;i<NB_MEM_PAGE;i++)
		printf("%d ",MemState[i].npage);
	printf("\n");
	printf("completed\n");
}


