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
- VMCtl(): A function that performs the specified memory operation on the virtual memory pages. It takes three arguments:
  - procID: The process ID of the remote process on which the operation is to be performed.
  - op: The memory operation to be performed.
  - range: A pointer to a Memory::Range object that describes the virtual memory pages to be operated upon.
  - VMCtlHandler(): A kernel handler function that performs the specified memory operation on the virtual memory pages. It takes the same arguments as VMCtl().
  
`kernel/API/VMCtl.cpp`: Implementing the function to make virtual memory related system calls.
```c++
    ProcessManager *procs = Kernel::instance()->getProcessManager();
    MemoryContext::Result memResult = MemoryContext::Success;
    API::Result ret = API::Success;
    Process *proc = ZERO;
```
In line 28 of file VMCtl.cpp the function first gets a pointer to the ProcessManager and uses it to find the process ID. The function retrieves the process's memory context and performs the requested memory operation on the virtual memory range specified in the range parameter. The supported memory operations include looking up the physical address corresponding to a given virtual address, mapping a contiguous or sparse virtual memory range to physical memory, unmapping a virtual memory range, releasing a virtual memory range, releasing sections of a virtual memory range, cleaning or invalidating the cache for a virtual memory range, and checking the access permissions for a virtual memory range. In each case, the function returns an appropriate error code if the operation fails for any reason. Finally, the function includes an implementation for reserving memory, which is done using a Allocator object.

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

`lib/liballoc/Allocator.cpp`: This C++ file implements the Allocator for dynamic memory allocation.
The Allocator provides an interface for allocating and releasing blocks of memory. The allocate function is a virtual function that must be implemented by derived classes to allocate memory, the release function will release memory, and the aligned function returns an address that is aligned to a specified boundary.
  
  
Advantages:

- Memory Protection: The memory management system, particularly virtual memory, can provide memory protection, preventing processes from accessing unauthorized memory regions.
- Memory Isolation: Virtual memory allows processes to have separate memory spaces, ensuring they do not interfere with each other's data or code.
- Efficient Memory Utilization: Techniques like paging and dynamic memory allocation enable efficient use of physical memory by allocating and releasing memory blocks as needed.
<br />
Disadvantages:

- Overhead: Due to page table lookups and address translation overhead may occur.
- Fragmentation: Dynamic memory allocation can lead to fragmentation, where memory blocks become scattered and fragmented over time, potentially leading to inefficient memory usage.
- Complexity: Memory management systems can be complex to implement and debug, requiring careful handling of various scenarios like memory leaks, synchronization, and concurrency.

## Conclusion

One of the key features of FreeNOS is its memory management system, which utilizes a combination of virtual memory, paging, and memory allocation techniques. By using virtual memory, FreeNOS is able to isolate logical memory addresses from physical memory addresses, allowing each process to have its own dedicated memory space. Paging divides physical memory into fixed-size pages, and virtual memory is divided into corresponding pages. Memory allocation is managed through techniques like heap management, where memory is allocated and released dynamically during runtime.

## References

OpenAI. (2021). ChatGPT [Chatbot model]. Retrieved from https://openai.com <br />
http://www.freenos.org/doxygen/classAllocator.html

