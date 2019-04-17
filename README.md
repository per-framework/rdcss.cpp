# [≡](#contents) [RDCSS.C++](#) [![Build Status](https://travis-ci.org/per-framework/rdcss.cpp.svg?branch=v1)](https://travis-ci.org/per-framework/rdcss.cpp)

[RDCSS](https://www.cl.cam.ac.uk/research/srg/netos/papers/2002-casn.pdf) based
multi-word compare-and-swap for C++.

See [`synopsis.hpp`](provides/include/rdcss_v1/synopsis.hpp) for the API.

**NOTE** that this is still very much unfinished work-in-progress!

## <a id="contents"></a> [≡](#contents) [Contents](#contents)

- [References](#references)

## <a id="references"></a> [≡](#contents) [References](#references)

- [A Practical Multi-Word Compare-and-Swap Operation](https://www.cl.cam.ac.uk/research/srg/netos/papers/2002-casn.pdf)
  - [Lock-Free Linked Lists Using Compare-and-Swap](http://people.csail.mit.edu/bushl2/rpi/portfolio/lockfree-grape/documents/lock-free-linked-lists.pdf)
  - [Easy Lock-Free Indexing in Non-Volatile Memory](http://justinlevandoski.org/papers/mwcas.pdf)
  - [Persistent Multi-Word Compare-and-Swap (PMwCAS) for NVRAM](https://github.com/Microsoft/pmwcas)
- [A Practical Wait-Free Multi-Word Compare-and-Swap Operation](https://www.osti.gov/servlets/purl/1110662)
- [Algorithms for Scalable Synchronization on Shared-Memory Multiprocessors](http://web.mit.edu/6.173/www/currentsemester/readings/R06-scalable-synchronization-1991.pdf)
- [Reagents: expressing and composing fine-grained concurrency](http://aturon.github.io/academic/reagents.pdf)
- [`std::atomic`](https://en.cppreference.com/w/cpp/atomic/atomic)
