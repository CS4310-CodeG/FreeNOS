# Inspecting FreeNOS Memory

FreeNOS is an open-source operating system that provides a modular and scalable platform for networt applications. FreeNOS utilizes virtual memory, paging, and memory allocation in its operating system to manage its memory. 

## FreeNOS Memory Management System : Discussion and Analysis

Virtual Memory: Used to separate logical and physical memory addresses, creating dedicated memory spaces for each process and improving memory protection, sharing, and efficiency.
<br />

`kernel/API/VMCtl.h`: This code defines an API to examine/modify virtual memory pages in a kernel. 
- MemoryOperation: an enumeration type that specifies the memory operation to be performed.
  - MapContiguous: Map a contiguous physical memory region to a contiguous virtual memory region.
  - MapSparse: Map a sparse physical memory region to a virtual memory region.
  - UnMap: Unmap a virtual memory region.
  - Release: Release physical memory pages.
  - ReleaseSections: Release multiple physical memory pages.
  - LookupVirtual: Lookup virtual memory address.
  - Access: Access virtual memory pages.
  - ReserveMem: Reserve memory pages.
  - AddMem: Add memory pages.
  - CacheClean: Clean the cache for a virtual memory region.
  - CacheInvalidate: Invalidate the cache for a virtual memory region.
  - CacheCleanInvalidate: Clean and invalidate the cache for a virtual memory region.
- VMCtl(): A function that performs the specified memory operation on the virtual memory pages.
  - procID: The process ID of the remote process on which the operation is to be performed.
  - op: The memory operation to be performed.
  - range: A pointer to a Memory::Range object that describes the virtual memory pages to be operated upon.
  - VMCtlHandler(): A kernel handler function that performs the specified memory operation on the virtual memory pages. It takes the same arguments as VMCtl().
"Description of the VMCtl.h file." ChatGPT, 5 May 2023.
 
`kernel/API/VMCtl.cpp`: Implementing the function to make virtual memory related system calls.
```c++
    ProcessManager *procs = Kernel::instance()->getProcessManager();
    MemoryContext::Result memResult = MemoryContext::Success;
    API::Result ret = API::Success;
    Process *proc = ZERO;
```
In file VMCtl.cpp, at line 28, the function finds the process ID by first obtaining a pointer to the ProcessManager. It later retrieves the memory context of the process and carries out the specified memory manipulation on the virtual memory range in the range parameter. For virtual memory ranges, supported memory operations are available. These include checking access permissions, releasing sections of a range, cleaning or invalidating the cache, and releasing or unmapping a range. Additionally, physical addresses that correspond to a virtual address can be looked up, and contiguous or sparse virtual memory ranges can be mapped to physical memory. If something goes wrong during the operation, the function will provide an error code. There is also a memory reservation implementation included that utilizes an Allocator object.

```c++
 case ReserveMem:
        {
            SplitAllocator *alloc = Kernel::instance()->getAllocator();
            Allocator::Result allocResult = Allocator::Success;

            for (Size i = 0; i < range->size; i += PAGESIZE)
            {
                const Address addr = range->phys + i;

                if (alloc->isAllocated(addr))
                {
                    ERROR("address " << (void *)addr << " is already allocated");
                    return API::InvalidArgument;
                }
                else if ((allocResult = alloc->allocate(addr)) != Allocator::Success)
                {
                    ERROR("failed to allocate " << (void *)addr << ", result = " << (int)allocResult);
                    return API::IOError;
                }
            }
            break;
        }
```



`kernel/Kernel.h`: Here we are defining a kernel class for the operating system. It defines a getter for the physical memory allocator. 

`kernel/Kernel.cpp`:Implementing the function.

In line 49, the SplitAllocator is a protected member variable for m_alloc.
```c++
   m_alloc  = new SplitAllocator(physRange, virtRange, PAGESIZE);

```
The getAllocator() function provides access to the SplitAllocator object pointer, which can be used to allocate and free physical memory.


`lib/libarch/MemoryMap.h`:

- MemoryMap(): default constructor that initializes memory ranges to zero.

- MemoryMap(const MemoryMap &map): copy constructor that creates a new MemoryMap object with the same memory ranges as map.

- Memory::Range range(Region region) const: returns the memory range for a given memory region.

- void setRange(Region region, Memory::Range range): sets the memory range for a given memory region.

"Description of the MemoryMap.h file." ChatGPT, 5 May 2023.

`lib/libarch/MemoryMap.cpp`: The MemoryMap function describes the virtual memory layout of the system.
In lines 84-95, 
```c++
  /**
     * Set memory range for the given region.
     *
     * @param region Identifier of the memory region to set
     * @param range New memory range value for the memory region
     */
    void setRange(Region region, Memory::Range range);

  protected:

    /** Memory ranges. */
    Memory::Range m_regions[MEMORYMAP_MAX_REGIONS];
```
The code defines a function setRange for the MemoryMap class to modify the memory range stored in m_regions for the specified region. It takes two parameters, region and range, and m_regions is a private data member that stores the memory ranges for each region with a fixed size of MEMORYMAP_MAX_REGIONS.

<br />
Paging: Dividing physical memory into fixed-size pages and dividing virtual memory into corresponding pages. The system would manage page tables to map virtual addresses to physical addresses.


`kernel/arm/sunxi-h3/Main.cpp`:
In line 63, the ARMPaging class is used to set up and manage page tables for the system.
```c++

    ARMPaging paging(&mem, (Address) &tmpPageDir, memoryBaseAddr);

```
FreeNOS uses paging to separate logical and physical memory addresses, which enables memory protection, sharing, and efficient utilization. This is done by creating an ARMPaging object with the memory map and base physical memory address, initializing the page tables, and activating the MMU.

We can also find Memoryblock and Arch::MemoryMap in this file. Memoryblock copies and sets memory blocks. Arch::MemoryMap is a namespace that contains functions for setting up the memory map for the system. It is used to set the base physical memory address for the system.

<br />
Memory allocation: FreeNOS uses dynamic memory allocation and deallocation. Memory is allocated and released during runtime.

`lib/liballoc/Allocator.h`: The allocator class provides memory allocation and release. It follows a parent-child heirarchy, where the parent allocator provides memory to the child allocator. If the child allocator needs more memory, it asks the parent for more.  

- Allocator(): Default class constructor.
- Allocator(const Range range): Class constructor with Range input.
- Allocator(): Class destructor.
- static void setDefault(Allocator *alloc): Makes the given Allocator the default.
- static Allocator *getDefault(): Retrieve the currently default Allocator.
- void setParent(Allocator *parent): Set parent allocator.
- Allocator * parent(): Get parent allocator.
- Address base() const: Get memory base address for allocations.
- Size alignment() const: Get memory alignment in bytes for allocations.
- virtual Size size() const: Get memory size.
- virtual Size available() const: Get memory available.
- virtual Result allocate(Range & range): Allocate memory.
- virtual Result release(const Address addr): Release memory.

"Description of the Allocator.h file." ChatGPT, 5 May 2023. 

  In the file of line 207, we can see where dynamic memory allocation occurs. 
```c++
  inline void * operator new(__SIZE_TYPE__ sz)
{
    Allocator::Range alloc_args;

    alloc_args.size = sz;
    alloc_args.alignment = 0;

    if (Allocator::getDefault()->allocate(alloc_args) == Allocator::Success)
        return (void *) alloc_args.address;
    else
        return (void *) NULL;
}
```
The "operator new" and "operator new[]" functions are used for dynamic memory allocation in C++.

`lib/liballoc/Allocator.cpp`: This C++ code implements an Allocator for dynamic memory allocation, which offers functions for allocating and releasing memory blocks. Derived classes must implement the allocate function, which allocates memory. The release function frees memory, while the aligned function returns an address that's aligned to a specified boundary.

"Description of the Allocator.cpp file." ChatGPT, 5 May 2023.
  
<br />
Advantages:

- Memory Protection: Virtual memory prevents unauthorized access to memory regions.
- Memory Isolation: Separate memory spaces ensure processes don't interfere.
- Efficient Memory Utilization: Techniques like paging and dynamic allocation optimize physical memory usage.

<br />
Disadvantages:

- Overhead: Address translation and page table lookups increase processing time.
- Fragmentation: Dynamic allocation may lead to inefficient memory usage due to scattered blocks.
- Complexity: Implementing and debugging memory management systems can be intricate, requiring careful handling of issues like concurrency and memory leaks.

## Conclusion

Utilizing a combination of memory allocation techniques, paging, and virtual memory is the memory management system of FreeNOS. One of its most significant features is how it can isolate physical memory addresses and logical memory addresses. This process grants each process its very own reserved memory space. The division of virtual memory into pages corresponds perfectly to the fixed-size pages of physical memory as created by paging. During runtime, memory is allocated and released dynamically through heap management techniques in order to handle memory allocation efficiently.

## References

OpenAI. (2021). ChatGPT [Chatbot model]. Retrieved from https://openai.com <br />
http://www.freenos.org/doxygen/classAllocator.html

